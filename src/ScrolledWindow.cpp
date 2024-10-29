#include <ScrolledWindow.hpp>

ScrolledWindow::ScrolledWindow(wxWindow *_parent) : wxHVScrolledWindow(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL) 
{
    this->Bind(wxEVT_MOTION, &ScrolledWindow::OnMouseEvent, this);
    this->Bind(wxEVT_MOUSEWHEEL, &ScrolledWindow::OnMouseEvent, this);

    m_scale = std::make_shared<int>(1);
    SetColumnCount(1);
    SetRowCount(1);
}

void ScrolledWindow::SetChildSpritePanel(std::shared_ptr<SpritePanel> _spritePanel)
{
    m_spritePanel = _spritePanel;
    m_spritePanel->Init(m_scale);
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

void ScrolledWindow::SetScale(int _scale)
{
    *m_scale = std::clamp<int>(_scale, 1, std::numeric_limits<int>::max());
    
    // Update the SpritePanel's minimum size, then use that to update my scrollbars
    wxSize _newMinSize = m_spritePanel->UpdateMinSize();
    SetColumnCount(_newMinSize.GetWidth());
    SetRowCount(_newMinSize.GetHeight());

    // Tell:
    //  1) the parent wxGridSizer to recenter its child (this)
    //  2) the wxScrolledWindow to update its scroll bars size/visibility
    this->GetParent()->Layout();
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
    auto mouseDelta = mousePosFrameRelative - prevMouseDragPos;

    // Scroll wheel zoom
    if(_event.GetWheelRotation() != 0)
    {
        int _newScale = *m_scale;
        if(_event.GetWheelRotation() > 0)
        {
            _newScale++;
        }
        else
        {
            _newScale--;
        }

        SetScale(_newScale);
    }

    // Left drag move sprite
    if(_event.GetEventType() == wxEVT_MOTION && _event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        wxLogStatus(wxString::Format("Mouse delta: (%i, %i)", mouseDelta.x, mouseDelta.y));
        m_spritePanel->ApplyPositionDelta(1.0f / static_cast<float>(*m_scale) * static_cast<float>(mouseDelta.x),
                                          1.0f / static_cast<float>(*m_scale) * static_cast<float>(mouseDelta.y));
    }

    // Canvas panning
    if(_event.GetEventType() == wxEVT_MOTION && _event.ButtonIsDown(wxMOUSE_BTN_RIGHT))
    {
        mouseDelta.x = std::clamp<int>(mouseDelta.x, -1, 1);
        mouseDelta.y = std::clamp<int>(mouseDelta.y, -1, 1);
        ScrollColumns(-mouseDelta.x);
        ScrollRows(-mouseDelta.y);
    }

    
    
    prevMouseDragPos = mousePosFrameRelative;

    _event.Skip();
}
