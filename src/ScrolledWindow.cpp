#include <ScrolledWindow.hpp>

ScrolledWindow::ScrolledWindow(wxWindow *_parent) : wxHVScrolledWindow(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL) 
{
    this->Bind(wxEVT_MOTION, &ScrolledWindow::OnMouseEvent, this);
    SetColumnCount(1);
    SetRowCount(1);
}

void ScrolledWindow::SetChildSpritePanel(std::shared_ptr<SpritePanel> _spritePanel)
{
    m_spritePanel = _spritePanel;
}

bool ScrolledWindow::LoadSprite(const std::filesystem::path &_path)
{
    bool _loaded = m_spritePanel->LoadSprite(_path);

    if(!_loaded)
    {
        return _loaded;
    }

    // The row and column count are the pixel size of the sprite panel
    SetColumnCount(m_spritePanel->GetSize().GetWidth());
    SetRowCount(m_spritePanel->GetSize().GetHeight());

    return _loaded;
}

wxCoord ScrolledWindow::OnGetRowHeight(size_t _row) const
{
    (void)_row;
    return 1;
}

wxCoord ScrolledWindow::OnGetColumnWidth(size_t _column) const
{
    (void)_column;
    return 1;
}

void ScrolledWindow::OnMouseEvent(wxMouseEvent &_event)
{
    wxPoint mousePosFrameRelative = this->ScreenToClient(wxGetMousePosition());

    if(_event.GetEventType() == wxEVT_MOTION && _event.ButtonIsDown(wxMOUSE_BTN_RIGHT))
    {
        auto mouseDelta = mousePosFrameRelative - prevMouseDragPos;
        mouseDelta.x = std::clamp<int>(mouseDelta.x, -1, 1);
        mouseDelta.y = std::clamp<int>(mouseDelta.y, -1, 1);
        wxLogStatus(wxString::Format("Mouse delta: (%i, %i)", mouseDelta.x, mouseDelta.y));
        ScrollColumns(-mouseDelta.x);
        ScrollRows(-mouseDelta.y);
    }
    
    prevMouseDragPos = mousePosFrameRelative;

    _event.Skip();
}
