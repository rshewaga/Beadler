#include <SpriteScrolledPanel.hpp>
#include <wx/utils.h>
#include <string>

SpriteScrolledPanel::SpriteScrolledPanel(wxWindow* _parent) : wxHVScrolledWindow(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS)
{
    // wxWANTS_CHARS is passed to the panel constructor so it intercepts navigation keys (tab, arrow keys).
    this->SetBackgroundColour(wxColour(255U,255U,255U,255U));
    this->SetDoubleBuffered(true);

    this->Bind(wxEVT_PAINT, &SpriteScrolledPanel::OnPaint, this);
    this->Bind(wxEVT_ERASE_BACKGROUND, &SpriteScrolledPanel::OnEraseBG, this);
    //wxEVT_LEFT_DOWN, wxEVT_LEFT_DOWN, wxEVT_MIDDLE_DOWN, wxEVT_LEFT_DCLICK
    this->Bind(wxEVT_MOTION, &SpriteScrolledPanel::OnMouseEvent, this);
    this->Bind(wxEVT_MOUSEWHEEL, &SpriteScrolledPanel::OnMouseEvent, this);
    this->Bind(wxEVT_KEY_DOWN, &SpriteScrolledPanel::OnKeyEvent, this);
    
    drawPos = {0,0};
}

bool SpriteScrolledPanel::LoadSprite(const std::filesystem::path &_path)
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
    //AutoFitCanvas();
    CenterSprite();
    
    this->SetRowColumnCount(minimumBoards[1] * boardSize.GetHeight(),minimumBoards[0] * boardSize.GetWidth());
    return true;
}

void SpriteScrolledPanel::CalculateMinimumBoards()
{
    minimumBoards = {static_cast<int>(std::ceil(static_cast<float>(loadedImg->GetWidth()) / static_cast<float>(boardSize.GetWidth()))),
                     static_cast<int>(std::ceil(static_cast<float>(loadedImg->GetHeight()) / static_cast<float>(boardSize.GetHeight())))};
}

void SpriteScrolledPanel::AutoFitCanvas()
{
    wxSize panelSize = this->GetSize();

    // Calculate the canvas scale needed so the # of minimum boards is perfectly shown in the parent panel.
    // The result is the minimum scale between fitting the minimum boards in width or height.
    scale = static_cast<float>(panelSize.GetWidth()) / static_cast<float>(minimumBoards[0]) / static_cast<float>(boardSize.GetWidth());
    scale = std::min<float>(scale, static_cast<float>(panelSize.GetHeight()) / static_cast<float>(minimumBoards[1]) / static_cast<float>(boardSize.GetHeight()));

    // Zoom out the smallest bit so the right and bottom edges of the drawn grid aren't cut off by the
    // extent of the parent panel
    scale *= 0.99f;
}

void SpriteScrolledPanel::CenterSprite()
{
    wxSize panelSize = this->GetSize();
    wxSize imgSize = loadedImg->GetSize();
    drawPos = {(minimumBoards[0] * boardSize.GetWidth() - loadedImg->GetWidth()) / 2.0f,
               (minimumBoards[1] * boardSize.GetHeight() - loadedImg->GetHeight()) / 2.0f};
    ClampSpritePos();
    CalculateNumBoardsHit();
    this->Refresh();
}

void SpriteScrolledPanel::ClampSpritePos()
{
    // Limit the sprite draw position to within the confines of the # number of boards
    drawPos[0] = std::clamp(drawPos[0], 0.0f, static_cast<float>(minimumBoards[0] * boardSize.GetWidth() - loadedImg->GetWidth()));
    drawPos[1] = std::clamp(drawPos[1], 0.0f, static_cast<float>(minimumBoards[1] * boardSize.GetHeight() - loadedImg->GetHeight()));
}

void SpriteScrolledPanel::OnPaint(wxPaintEvent &_event)
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
        dc.SetUserScale(scale, scale);
        dc.DrawBitmap(*loadedBMP.get(), wxPoint(drawPos[0], drawPos[1]));
    }

    DrawBoardEdges(dc);
}

void SpriteScrolledPanel::DrawBoardFills(wxBufferedPaintDC &_dc)
{
    // User scale set to 1 so the grid lines are pixel perfect width of 1
    _dc.SetUserScale(1.0f, 1.0f);

    wxBrush _whiteBrush = _dc.GetBrush();
    _whiteBrush.SetColour(255,255,255);
    wxBrush _greyBrush = _dc.GetBrush();
    _greyBrush.SetColour(200,200,200);

    _dc.SetFont(_dc.GetFont().Scaled(scale * 0.3f));

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

            int _drawPosX = column * boardSize.GetWidth() * scale;
            int _drawPosY = row * boardSize.GetHeight() * scale;
            _dc.DrawRectangle(_drawPosX, _drawPosY, 
                              boardSize.GetWidth() * scale + 1, 
                              boardSize.GetHeight() * scale + 1);
            _dc.DrawText(wxString::Format(" %i = (%i, %i)", _boardID, row, column),
                         _drawPosX, _drawPosY);
        }
    }
}

void SpriteScrolledPanel::DrawBoardEdges(wxBufferedPaintDC& _dc)
{
    // User scale set to 1 so the grid lines are pixel perfect width of 1
    _dc.SetUserScale(1.0f, 1.0f);

    // The "+1" in the below draw calls are used due to the DrawLine's end position being exclusive.
    // Adding 1 pixel causes the entire desired line to be drawn.
    
    for(int row = 0; row <= minimumBoards[1]; ++row)
    {
        _dc.DrawLine(0,                                                   row * boardSize.GetHeight() * scale,
                     minimumBoards[0] * boardSize.GetWidth() * scale + 1, row * boardSize.GetHeight() * scale);

        for(int column = 0; column <= minimumBoards[0]; ++column)
        {
            _dc.DrawLine(column * boardSize.GetWidth() * scale, 0,
                         column * boardSize.GetWidth() * scale, minimumBoards[1] * boardSize.GetHeight() * scale + 1);
        }
    }
}

void SpriteScrolledPanel::OnEraseBG(wxEraseEvent &_event)
{
    // This is explicitly defined so that the wxBufferedPaintDC 
    // in OnPaint can render double buffered and prevent flickering.
    (void)_event;
}

void SpriteScrolledPanel::OnMouseEvent(wxMouseEvent &_event)
{
    wxPoint mousePosRelativeToHoveredControl = _event.GetPosition();
    wxPoint mousePosScreenCoords = wxGetMousePosition();
    wxPoint mousePosFrameRelative = this->ScreenToClient(wxGetMousePosition());
    // wxWindow::ScreenToClient, ::ClientToScreen
    //drawPos = mousePosFrameRelative;


    // Scroll wheel zoom
    if(_event.GetWheelRotation() > 0)
    {
        scale += 1;
    }
    else if (_event.GetWheelRotation() < 0)
    {
        scale = std::clamp(scale - 1, 1, std::numeric_limits<int>::max());
    }

    // Motion
    if(_event.GetEventType() == wxEVT_MOTION && _event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        drawPos[0] += 1.0f / scale * (mousePosFrameRelative - prevMouseDragPos).x;
        drawPos[1] += 1.0f / scale * (mousePosFrameRelative - prevMouseDragPos).y;
    }
    ClampSpritePos();
    CalculateNumBoardsHit();

    if(_event.GetEventType() == wxEVT_MOTION && _event.ButtonIsDown(wxMOUSE_BTN_RIGHT))
    {
        auto mouseDelta = mousePosFrameRelative - prevMouseDragPos;
        mouseDelta.x = std::clamp<int>(mouseDelta.x, -1, 1);
        mouseDelta.y = std::clamp<int>(mouseDelta.y, -1, 1);
        wxLogStatus(wxString::Format("Mouse delta: (%i, %i)", mouseDelta.x, mouseDelta.y));
        ScrollColumns(-mouseDelta.x);
        ScrollRows(-mouseDelta.y);
    }

    //wxString log = wxString::Format("Mouse event detected! (%d, %d)", mousePosFrameRelative.x, mousePosFrameRelative.y);
    //wxLogStatus(log);

    prevMouseDragPos = mousePosFrameRelative;

    wxVarHScrollHelper::RefreshAll();
    wxVarVScrollHelper::RefreshAll();
    this->Refresh();

    //wxLogStatus(wxString::Format("Scale: %i | Width: %i | Height: %i",
    //                            scale, EstimateTotalWidth(), EstimateTotalHeight()));

    _event.Skip();
}

void SpriteScrolledPanel::OnKeyEvent(wxKeyEvent &_event)
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

int SpriteScrolledPanel::CalculateNumBoardsHit()
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

wxCoord SpriteScrolledPanel::OnGetRowHeight(size_t row) const
{
    (void)row;
    return wxCoord(1 * scale);
}

wxCoord SpriteScrolledPanel::OnGetColumnWidth(size_t col) const
{
    (void)col;
    return wxCoord(1 * scale);
}
