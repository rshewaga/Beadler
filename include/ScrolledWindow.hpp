#pragma once

#include <wx/vscroll.h>
#include <SpritePanel.hpp>

class ScrolledWindow : public wxHVScrolledWindow
{
public:
    ScrolledWindow(wxWindow* _parent);
    void SetChildSpritePanel(std::shared_ptr<SpritePanel> _spritePanel);
    
    /**
     * @brief Set the displayed sprite to the given image at the absolute file path.
     * @param _path Absolute path to the image to load
     * @return Success or failure loading the sprite
     */
    bool LoadSprite(const std::filesystem::path& _path);

    /**
     * @brief Set the scale of the child SpritePanel
     * @param _scale Clamped to >0
     */
    void SetScale(int _scale);

private:
    wxCoord OnGetRowHeight(size_t _row) const override;
    wxCoord OnGetColumnWidth(size_t _column) const override;

    /**
     * @brief Capture mouse events to pan the window
     * @param _event
     */
    void OnMouseEvent(wxMouseEvent& _event);

    std::shared_ptr<SpritePanel> m_spritePanel;
    // The previous frame's mouse position when left click dragging
    wxPoint prevMouseDragPos = wxPoint();

    // Scale of the SpritePanel. Pointer is shared with the SpritePanel for its drawing.
    std::shared_ptr<int> m_scale;
};