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

    return true;
}

void SpritePanel::CalculateMinimumBoards()
{
    minimumBoards = {static_cast<int>(std::ceil(static_cast<float>(loadedImg->GetWidth()) / static_cast<float>(boardSize.GetWidth()))),
                     static_cast<int>(std::ceil(static_cast<float>(loadedImg->GetHeight()) / static_cast<float>(boardSize.GetHeight())))};
}

wxSize SpritePanel::UpdateMinSize()
{
    // +1 added here so the right and bottom edges of the panel can include drawing the minimumBoard's edges.
    this->SetMinSize({minimumBoards[0] * boardSize.GetWidth() * *m_scale + 1, 
                      minimumBoards[1] * boardSize.GetHeight() * *m_scale + 1});
                      
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
        CalculateNumBoardsHit();
        this->Refresh();
    }
}

void SpritePanel::AutoFitCanvas()
{
    wxSize scrollableWinSize = this->GetParent()->GetSize();

    // Calculate the canvas scale needed so the # of minimum boards is maximized in
    // the parent panel without the scroll bars being shown.
    // The result is the minimum scale between fitting the minimum boards in width or height.
    *m_scale = static_cast<float>(scrollableWinSize.GetWidth()) / static_cast<float>(minimumBoards[0]) / static_cast<float>(boardSize.GetWidth());
    *m_scale = std::min<float>(*m_scale, static_cast<float>(scrollableWinSize.GetHeight()) / static_cast<float>(minimumBoards[1]) / static_cast<float>(boardSize.GetHeight()));

    UpdateMinSize();
}

void SpritePanel::CenterSprite()
{
    wxSize panelSize = this->GetSize();
    wxSize imgSize = loadedImg->GetSize();
    drawPos = {(minimumBoards[0] * boardSize.GetWidth() - loadedImg->GetWidth()) / 2.0f,
               (minimumBoards[1] * boardSize.GetHeight() - loadedImg->GetHeight()) / 2.0f};
    
    ClampSpritePos();
    CalculateNumBoardsHit();

    this->Refresh();
}

void SpritePanel::ClampSpritePos()
{
    // Limit the sprite draw position to within the confines of the number of boards drawn
    drawPos[0] = std::clamp(drawPos[0], 0.0f, static_cast<float>(minimumBoards[0] * boardSize.GetWidth() - loadedImg->GetWidth()));
    drawPos[1] = std::clamp(drawPos[1], 0.0f, static_cast<float>(minimumBoards[1] * boardSize.GetHeight() - loadedImg->GetHeight()));
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

    for(int row = 0; row < minimumBoards[1]; ++row)
    {
        //_dc.DrawLine(0,                                                   row * boardSize.GetHeight() * scale,
        //             minimumBoards[0] * boardSize.GetWidth() * scale + 1, row * boardSize.GetHeight() * scale);

        for(int column = 0; column < minimumBoards[0]; ++column)
        {
            //_dc.DrawLine(column * boardSize.GetWidth() * scale, 0,
            //             column * boardSize.GetWidth() * scale, minimumBoards[1] * boardSize.GetHeight() * scale + 1);

            // If this board is hit by the sprite, color the box
            int _boardID = row * minimumBoards[0] + column;
            if(boardsHit.contains(_boardID))
            {
                _dc.SetBrush(_greyBrush);
            }
            else
            {
                _dc.SetBrush(_whiteBrush);
            }

            int _drawPosX = column * boardSize.GetWidth() * *m_scale;
            int _drawPosY = row * boardSize.GetHeight() * *m_scale;
            _dc.DrawRectangle(_drawPosX, _drawPosY, 
                              boardSize.GetWidth() * *m_scale + 1, 
                              boardSize.GetHeight() * *m_scale + 1);
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
    
    for(int row = 0; row <= minimumBoards[1]; ++row)
    {
        _dc.DrawLine(0,                                                   row * boardSize.GetHeight() * *m_scale,
                     minimumBoards[0] * boardSize.GetWidth() * *m_scale + 1, row * boardSize.GetHeight() * *m_scale);

        for(int column = 0; column <= minimumBoards[0]; ++column)
        {
            _dc.DrawLine(column * boardSize.GetWidth() * *m_scale, 0,
                         column * boardSize.GetWidth() * *m_scale, minimumBoards[1] * boardSize.GetHeight() * *m_scale + 1);
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
        wxLogStatus(wxString::Format("Mouse delta: (%i, %i)", mouseDelta.x, mouseDelta.y));
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
    CalculateNumBoardsHit();
    this->Refresh();
}

int SpritePanel::CalculateNumBoardsHit()
{
    boardsHit.clear();

    // For each board in minimumBoards, flood fill from the top left
    // until an opaque pixel is hit in the sprite.

    // If the image doesn't have an alpha, then every board of the
    // minimum boards needed is being hit.
    if(!loadedImg->HasAlpha())
    {
        for(int i = 0; i < minimumBoards[0] * minimumBoards[1]; ++i)
        {
            boardsHit.emplace(i);
        }
        return boardsHit.size();
    }

    bool _boardHit = false;
    int _testPixelX = 0;
    int _testPixelY = 0;
    for(int boardX = 0; boardX < minimumBoards[0]; ++boardX)
    {
        for(int boardY = 0; boardY < minimumBoards[1]; ++boardY)
        {
            _boardHit = false;
            for(int pixelX = 0; pixelX < boardSize.GetWidth(); ++pixelX)
            {
                for(int pixelY = 0; pixelY < boardSize.GetWidth(); ++pixelY)
                {
                    _testPixelX = boardX * boardSize.GetWidth() + pixelX - static_cast<int>(drawPos[0]);
                    _testPixelY = boardY * boardSize.GetHeight() + pixelY - static_cast<int>(drawPos[1]);

                    // Check if the test pixel is on the sprite.
                    // It can be off due to the sprite moving on the boards.
                    if(_testPixelX < 0 || _testPixelY < 0 || _testPixelX >= loadedImg->GetWidth() || _testPixelY >= loadedImg->GetHeight())
                    {
                        continue;
                    }

                    if(!loadedImg->IsTransparent(_testPixelX, _testPixelY))
                    {
                        boardsHit.emplace(boardY * minimumBoards[0] + boardX);
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
        for(auto _ID : boardsHit)
        {
            _log += std::to_string(_ID) + " ";
        }
        wxLogStatus(wxString(_log));
    }

    return boardsHit.size();
}
