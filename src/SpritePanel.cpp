#include <SpritePanel.hpp>
#include <wx/utils.h>
#include <string>

SpritePanel::SpritePanel(wxWindow* _parent) : wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)//, wxWANTS_CHARS)
{
    // wxWANTS_CHARS is passed to the panel constructor so it intercepts navigation keys (tab, arrow keys).
    this->SetBackgroundColour(wxColour(255U,255U,255U,255U));
    this->SetDoubleBuffered(true);

    this->Bind(wxEVT_PAINT, &SpritePanel::OnPaint, this);
    this->Bind(wxEVT_ERASE_BACKGROUND, &SpritePanel::OnEraseBG, this);
    this->Bind(wxEVT_MOTION, &SpritePanel::OnMouseEvent, this);
    //this->Bind(wxEVT_MOUSEWHEEL, &SpritePanel::OnMouseEvent, this);
    this->Bind(wxEVT_KEY_DOWN, &SpritePanel::OnKeyEvent, this);

    drawPos = {0,0};

    Dispatcher::Get().sink<Event_JobStateChanged>().connect<&SpritePanel::OnJobStateChanged>(this);
}

void SpritePanel::Init(std::shared_ptr<int> _scale)
{
    m_scale = _scale;
    UpdateMinSize();
}

bool SpritePanel::LoadSprite(const std::filesystem::path &_path)
{
    if(loadedImg != nullptr)
    {
        loadedImg.reset();
    }

    loadedImg = std::make_shared<wxImage>();

    {
        // Suppress common "incorrect sRGB format" warning when loading PNGs
        wxLogNull logno;
        (void)logno;

        loadedImg->LoadFile(_path.string());
    }

    if(!loadedImg->IsOk())
    {
        wxLogError("Converting selected file to wxImage failed!");
        return false;
    }
    
    if(loadedBMP != nullptr)
    {
        loadedBMP.reset();
    }
    loadedBMP = std::make_shared<wxBitmap>(*loadedImg.get());

    CalculateMinimumBoards();
    AutoFitCanvas();
    CenterSprite();

    Job* _hitBoardJob = JobManager::Inst().GetJobByID(JobManager::Inst().CreateJob("Calculate hit boards"));
    _hitBoardJob->Begin(&SpritePanel::Thread_CalculateBoardsHit, this, _hitBoardJob);
    m_calculateHitBoardsJobID = _hitBoardJob->m_ID;

    return true;
}

void SpritePanel::CalculateMinimumBoards()
{
    m_minimumBoards = {static_cast<int>(std::ceil(static_cast<float>(loadedImg->GetWidth()) / static_cast<float>(m_boardSize.GetWidth()))),
                     static_cast<int>(std::ceil(static_cast<float>(loadedImg->GetHeight()) / static_cast<float>(m_boardSize.GetHeight())))};
}

wxSize SpritePanel::UpdateMinSize()
{
    // +1 added here so the right and bottom edges of the panel can include drawing the minimumBoard's edges.
    this->SetMinSize({m_minimumBoards[0] * m_boardSize.GetWidth() * *m_scale + 1, 
                      m_minimumBoards[1] * m_boardSize.GetHeight() * *m_scale + 1});
                      
    wxLogStatus(wxString::Format("MinSize updated to: (%i, %i)", this->GetMinSize().GetWidth(), this->GetMinSize().GetHeight()));
    
    return this->GetMinSize();
}

void SpritePanel::ApplyPositionDelta(float _x, float _y)
{
    // Caching the current drawPos to compare whether any screen movement actually happened.
    // Used to prevent expensive CalculateNumBoardsHit and redraw when unnecessary.
    std::array<int, 2> _oldPos = {static_cast<int>(drawPos[0]), static_cast<int>(drawPos[1])};

    drawPos[0] += _x;
    drawPos[1] += _y;
    ClampSpritePos();

    // If the movement was large enough to move the sprite as drawn
    if(static_cast<int>(drawPos[0]) != _oldPos[0] || static_cast<int>(drawPos[1]) != _oldPos[1])
    {
        TriggerCalculateNumBoardsHit();
        this->Refresh();
    }
}

void SpritePanel::TriggerCalculateNumBoardsHit()
{
    wxLogStatus("Triggering job to recalculate");
    m_needsBoardsHitUpdate.test_and_set();
}

void SpritePanel::Thread_CalculateBoardsHit(Job* _job)
{
    // Create a vector of alpha values representing the alpha of each pixel.
    // If the image doesn't have alpha, the vector is filled with opaque pixels.
    // If the image has an alpha channel, copy its data locally to the vector.
    int _imgWidth = loadedImg->GetWidth();
    int _imgHeight = loadedImg->GetHeight();
    int _numPixels = _imgWidth * _imgHeight;
    std::vector<unsigned char> _alpha(_numPixels, 255U);    // Initialized with all opaque pixels
    
    if(loadedImg->HasAlpha())
    {
        unsigned char* _sourceAlpha = loadedImg->GetAlpha();
        for(int i = 0; i < _numPixels; ++i)
        {
            _alpha[i] = _sourceAlpha[i];
        }
    }

    while(!_job->m_thread.get_stop_token().stop_requested())
    {
        if(!m_needsBoardsHitUpdate.test())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        wxLogStatus("Recalculating...");

        // Clear the atomic flag immediately so this thread can periodically check it
        // and cancel out it's work early to restart.
        // Example: began work, but sprite position moved. Restart work.
        m_needsBoardsHitUpdate.clear();
        
        // Boards hit needs to be recalculated
        _job->SetProgress(0);
        _job->SetState(Job::STATE::WORKING);

        // Flag to restart the outer while loop due to sprite state
        // changing during processing (e.g. position changed)
        bool _restartJob = false;

        std::set<int> _boardsHit = std::set<int>();

        /*
        for(int i = 0; i < 100; ++i)
        {
            _job->SetProgress(static_cast<float>(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            if(m_needsBoardsHitUpdate.test())
                break;
        }
        */

        bool _boardHit = false;
        int _testPixelX = 0;
        int _testPixelY = 0;

        // Cache local copies of panel members
        std::array<int, 2> _minimumBoards = m_minimumBoards;
        wxSize _boardSize = m_boardSize;

        // Internally track the completion progress.
        // Only post the progress update to the job after a threshold is passed in order to reduce event spamming.
        float _progress = 0.0f;
        float _lastProgressChange = 0.0f;
        float _progressChangeThresholdForEvent = 5.0f;
        float _perBoardProgress = 100.0f / static_cast<float>(_minimumBoards[0] * _minimumBoards[1]);
        float _perBoardPixelProgress = _perBoardProgress / static_cast<float>(_minimumBoards[0] * _minimumBoards[1]);

        for(int boardX = 0; boardX < _minimumBoards[0]; ++boardX)
        {
            for(int boardY = 0; boardY < _minimumBoards[1]; ++boardY)
            {
                _boardHit = false;
                for(int pixelX = 0; pixelX < _boardSize.GetWidth(); ++pixelX)
                {
                    for(int pixelY = 0; pixelY < _boardSize.GetWidth(); ++pixelY)
                    {
                        // Update job progress if enough progress has been made
                        if(_progress >= _lastProgressChange + _progressChangeThresholdForEvent)
                        {
                            _lastProgressChange = _progress;
                            _job->SetProgress(_progress);
                        }

                        _testPixelX = boardX * _boardSize.GetWidth() + pixelX - static_cast<int>(drawPos[0]);
                        _testPixelY = boardY * _boardSize.GetHeight() + pixelY - static_cast<int>(drawPos[1]);

                        // Calculate progress
                        _progress = _perBoardProgress * (boardY * _minimumBoards[0] + boardX) + _perBoardPixelProgress;

                        // Check if the test pixel is on the sprite.
                        // It can be off due to the sprite moving on the boards.
                        if(_testPixelX < 0 || _testPixelY < 0 || _testPixelX >= _imgWidth || _testPixelY >= _imgHeight)
                        {
                            continue;
                        }

                        if(_alpha[_testPixelY * _imgWidth + _testPixelX] > 200U)
                        {
                            _boardsHit.emplace(boardY * _minimumBoards[0] + boardX);
                            _boardHit = true;
                            break;
                        }
                    }

                    if(m_needsBoardsHitUpdate.test())
                    {
                        _restartJob = true;
                        break;
                    }

                    if(_boardHit)
                    {
                        break;
                    }
                }

                if(_restartJob)
                {
                    break;
                }
            }

            if(_restartJob)
            {
                break;
            }
        }
        if(_restartJob)
        {
            wxLogStatus("Restarting calculating due to state change");
            continue;
        }

        // Apply the results
        {
            std::lock_guard<std::mutex> _lock(m_hitMutex);
            m_boardsHit = _boardsHit;
        }

        // Work finished
        _job->SetProgress(100);
        _job->SetState(Job::STATE::WAITING);

        wxLogStatus("Recalculating finished");
    }

    (void)_alpha;
}

void SpritePanel::OnJobStateChanged(const Event_JobStateChanged &_event)
{
    const Job* _job = JobManager::Inst().GetConstJobByID(_event.m_jobID);

    if(_job->m_ID == m_calculateHitBoardsJobID)
        this->Refresh();   // Refresh UI
}

void SpritePanel::AutoFitCanvas()
{
    wxSize scrollableWinSize = this->GetParent()->GetSize();

    // Calculate the canvas scale needed so the # of minimum boards is maximized in
    // the parent panel without the scroll bars being shown.
    // The result is the minimum scale between fitting the minimum boards in width or height.
    *m_scale = static_cast<float>(scrollableWinSize.GetWidth()) / static_cast<float>(m_minimumBoards[0]) / static_cast<float>(m_boardSize.GetWidth());
    *m_scale = std::min<float>(*m_scale, static_cast<float>(scrollableWinSize.GetHeight()) / static_cast<float>(m_minimumBoards[1]) / static_cast<float>(m_boardSize.GetHeight()));

    UpdateMinSize();
}

void SpritePanel::CenterSprite()
{
    wxSize panelSize = this->GetSize();
    wxSize imgSize = loadedImg->GetSize();
    drawPos = {(m_minimumBoards[0] * m_boardSize.GetWidth() - loadedImg->GetWidth()) / 2.0f,
               (m_minimumBoards[1] * m_boardSize.GetHeight() - loadedImg->GetHeight()) / 2.0f};
    
    ClampSpritePos();
    TriggerCalculateNumBoardsHit();

    this->Refresh();
}

void SpritePanel::ClampSpritePos()
{
    // Limit the sprite draw position to within the confines of the number of boards drawn
    drawPos[0] = std::clamp(drawPos[0], 0.0f, static_cast<float>(m_minimumBoards[0] * m_boardSize.GetWidth() - loadedImg->GetWidth()));
    drawPos[1] = std::clamp(drawPos[1], 0.0f, static_cast<float>(m_minimumBoards[1] * m_boardSize.GetHeight() - loadedImg->GetHeight()));
}

void SpritePanel::OnPaint(wxPaintEvent &_event)
{
    (void)_event;
    //wxLogStatus("Paint");

    wxBufferedPaintDC dc(this);
    PrepareDC(dc);
    dc.Clear();
    
    //dc.DrawText(wxString::Format("Scale: %f", scale), 0, 0);
    //dc.DrawText(wxString::Format("Pos: (%f, %f)", drawPos[0], drawPos[1]), 0, 30);
    DrawBoardFills(dc);
    if(loadedBMP != nullptr)
    {
        //wxLogStatus("Paint the BMP");
        dc.SetUserScale(*m_scale, *m_scale);
        dc.DrawBitmap(*loadedBMP.get(), wxPoint(drawPos[0], drawPos[1]));
    }

    DrawBoardEdges(dc);
}

void SpritePanel::DrawBoardFills(wxBufferedPaintDC &_dc)
{
    // User scale set to 1 so the grid lines are pixel perfect width of 1
    _dc.SetUserScale(1.0f, 1.0f);

    wxBrush _whiteBrush = _dc.GetBrush();
    _whiteBrush.SetColour(255,255,255);
    wxBrush _greyBrush = _dc.GetBrush();
    _greyBrush.SetColour(200,200,200);

    _dc.SetFont(_dc.GetFont().Scaled(*m_scale * 0.3f));

    for(int row = 0; row < m_minimumBoards[1]; ++row)
    {
        //_dc.DrawLine(0,                                                   row * boardSize.GetHeight() * scale,
        //             minimumBoards[0] * boardSize.GetWidth() * scale + 1, row * boardSize.GetHeight() * scale);

        for(int column = 0; column < m_minimumBoards[0]; ++column)
        {
            //_dc.DrawLine(column * boardSize.GetWidth() * scale, 0,
            //             column * boardSize.GetWidth() * scale, minimumBoards[1] * boardSize.GetHeight() * scale + 1);

            // If this board is hit by the sprite, color the box
            int _boardID = row * m_minimumBoards[0] + column;
            if(m_boardsHit.contains(_boardID))
            {
                _dc.SetBrush(_greyBrush);
            }
            else
            {
                _dc.SetBrush(_whiteBrush);
            }

            int _drawPosX = column * m_boardSize.GetWidth() * *m_scale;
            int _drawPosY = row * m_boardSize.GetHeight() * *m_scale;
            _dc.DrawRectangle(_drawPosX, _drawPosY, 
                              m_boardSize.GetWidth() * *m_scale + 1, 
                              m_boardSize.GetHeight() * *m_scale + 1);
            _dc.DrawText(wxString::Format(" %i = (%i, %i)", _boardID, row, column),
                         _drawPosX, _drawPosY);
        }
    }
}

void SpritePanel::DrawBoardEdges(wxBufferedPaintDC& _dc)
{
    // User scale set to 1 so the grid lines are pixel perfect width of 1
    _dc.SetUserScale(1.0f, 1.0f);

    // The "+1" in the below draw calls are used due to the DrawLine's end position being exclusive.
    // Adding 1 pixel causes the entire desired line to be drawn.
    
    for(int row = 0; row <= m_minimumBoards[1]; ++row)
    {
        _dc.DrawLine(0,                                                   row * m_boardSize.GetHeight() * *m_scale,
                     m_minimumBoards[0] * m_boardSize.GetWidth() * *m_scale + 1, row * m_boardSize.GetHeight() * *m_scale);

        for(int column = 0; column <= m_minimumBoards[0]; ++column)
        {
            _dc.DrawLine(column * m_boardSize.GetWidth() * *m_scale, 0,
                         column * m_boardSize.GetWidth() * *m_scale, m_minimumBoards[1] * m_boardSize.GetHeight() * *m_scale + 1);
        }
    }
}

void SpritePanel::OnEraseBG(wxEraseEvent &_event)
{
    // This is explicitly defined so that the wxBufferedPaintDC 
    // in OnPaint can render double buffered and prevent flickering.
    (void)_event;
}

void SpritePanel::OnMouseEvent(wxMouseEvent &_event)
{
    _event.Skip();
    wxPoint mousePosFrameRelative = this->ScreenToClient(wxGetMousePosition());

    // Left drag move sprite
    if(_event.GetEventType() == wxEVT_MOTION && _event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        wxPoint mouseDelta = mousePosFrameRelative - prevMouseDragPos;
        //wxLogStatus(wxString::Format("Mouse delta: (%i, %i)", mouseDelta.x, mouseDelta.y));
        ApplyPositionDelta(1.0f / static_cast<float>(*m_scale) * static_cast<float>(mouseDelta.x),
                           1.0f / static_cast<float>(*m_scale) * static_cast<float>(mouseDelta.y));
    }

    prevMouseDragPos = mousePosFrameRelative;

    //_event.Skip();
    //this->Refresh();
}

void SpritePanel::OnKeyEvent(wxKeyEvent &_event)
{
    // The panel is capturing special navigation keys (tab, arrow keys).
    // If tab is pressed, use it as a regular control navigation event.
    if(_event.GetKeyCode() == WXK_TAB)
    {
        wxWindow* _window = (wxWindow*)_event.GetEventObject();
        _window->Navigate();
    }
    
    // Move the sprite by the arrow keys.
    // Hold shift to move faster.
    float posDelta = 1.0f;
    if(wxGetKeyState(WXK_SHIFT))
    {
        posDelta = 5.0f;
    }
    switch(_event.GetKeyCode())
    {
        case WXK_LEFT:
            drawPos[0] -= posDelta;
            break;
        case WXK_RIGHT:
            drawPos[0] += posDelta;
            break;
        case WXK_UP:
            drawPos[1] -= posDelta;
            break;
        case WXK_DOWN:
            drawPos[1] += posDelta;
            break;
    }

    ClampSpritePos();
    TriggerCalculateNumBoardsHit();
    this->Refresh();
}

int SpritePanel::CalculateNumBoardsHit()
{
    m_boardsHit.clear();

    // For each board in minimumBoards, flood fill from the top left
    // until an opaque pixel is hit in the sprite.

    // If the image doesn't have an alpha, then every board of the
    // minimum boards needed is being hit.
    if(!loadedImg->HasAlpha())
    {
        for(int i = 0; i < m_minimumBoards[0] * m_minimumBoards[1]; ++i)
        {
            m_boardsHit.emplace(i);
        }
        return m_boardsHit.size();
    }

    bool _boardHit = false;
    int _testPixelX = 0;
    int _testPixelY = 0;
    for(int boardX = 0; boardX < m_minimumBoards[0]; ++boardX)
    {
        for(int boardY = 0; boardY < m_minimumBoards[1]; ++boardY)
        {
            _boardHit = false;
            for(int pixelX = 0; pixelX < m_boardSize.GetWidth(); ++pixelX)
            {
                for(int pixelY = 0; pixelY < m_boardSize.GetWidth(); ++pixelY)
                {
                    _testPixelX = boardX * m_boardSize.GetWidth() + pixelX - static_cast<int>(drawPos[0]);
                    _testPixelY = boardY * m_boardSize.GetHeight() + pixelY - static_cast<int>(drawPos[1]);

                    // Check if the test pixel is on the sprite.
                    // It can be off due to the sprite moving on the boards.
                    if(_testPixelX < 0 || _testPixelY < 0 || _testPixelX >= loadedImg->GetWidth() || _testPixelY >= loadedImg->GetHeight())
                    {
                        continue;
                    }

                    if(!loadedImg->IsTransparent(_testPixelX, _testPixelY))
                    {
                        m_boardsHit.emplace(boardY * m_minimumBoards[0] + boardX);
                        _boardHit = true;
                        break;
                    }
                }

                if(_boardHit)
                {
                    break;
                }
            }
        }
    }

    if(false)
    {
        std::string _log = "Boards hit: ";
        for(auto _ID : m_boardsHit)
        {
            _log += std::to_string(_ID) + " ";
        }
        wxLogStatus(wxString(_log));
    }

    return m_boardsHit.size();
}
