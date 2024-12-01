#pragma once

#include <filesystem>
#include <array>
#include <set>
#include <atomic>
#include <mutex>

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/vscroll.h>

#include <JobManager.hpp>
#include <Job.hpp>

/**
 * @brief A panel for displaying and moving a sprite in reference to a grid.
 */
class SpritePanel : public wxPanel
{
public:
    SpritePanel(wxWindow* _parent);

    /**
     * @brief Init called from the parent ScrolledWindow
     * @param _scale Render scale stored in the parent
     */
    void Init(std::shared_ptr<int> _scale);

    /**
     * @brief Set the displayed sprite to the given image at the absolute file path.
     * @param _path Absolute path to the image to load
     * @return Success or failure loading the sprite
     */
    bool LoadSprite(const std::filesystem::path& _path);

    /**
     * @brief Calculate the minimum number of boards to fit the sprite, based on sprite dimensions and board dimensions
     */
    void CalculateMinimumBoards();

    /**
     * Calculate and set the canvas scale so the minimumBoards are shown
     */
    void AutoFitCanvas();

    /**
     * @brief Centers the sprite in the canvas
     */
    void CenterSprite();

    /**
     * @brief Clamp the sprite's position so it entirely fits within the minimumBoards
     */
    void ClampSpritePos();
    
    void OnPaint(wxPaintEvent& _event);
    
    /**
     * @brief Draw the minimumBoards as colored rectangles. Hit boards are grey.
     * @param _dc The panel's used draw context
     */
    void DrawBoardFills(wxBufferedPaintDC& _dc);

    /**
     * @brief Draw the outlines of the minimumBoards as lines
     * @param _dc The panel's used draw context
     */
    void DrawBoardEdges(wxBufferedPaintDC& _dc);

    /**
     * @brief Capture the panel's erase event to prevent flickering
     * @param _event
     */
    void OnEraseBG(wxEraseEvent& _event);

    /**
     * @brief Capture mouse events to move the sprite by holding left click
     * @param _event
     */
    void OnMouseEvent(wxMouseEvent& _event);

    /**
     * @brief Intercept keyboard events to use the arrow keys to move the sprite
     * @param _event
     */
    void OnKeyEvent(wxKeyEvent& _event);

    /**
     * @brief Calculate the number of boards currently hit by any part of the sprite
     * @return The # of boards currently hit by any part of the sprite
     */
    int CalculateNumBoardsHit();

    /**
     * @brief Update the minimum size of the panel based on its scale
     * @return The new minimum size
     */
    wxSize UpdateMinSize();

    /**
     * @brief Apply a translation to the position
     * @param _x X position delta, right positive
     * @param _y Y position delta, down positive
     */
    void ApplyPositionDelta(float _x, float _y);

    /**
     * @brief Trigger the boardsHit thread to recalculate which boards are hit
     */
    void TriggerCalculateNumBoardsHit();
    
    // The loaded sprite in raw wxImage format
    std::shared_ptr<wxImage> loadedImg;
    // The loaded sprite converted to platform-specific wxBitmap
    std::shared_ptr<wxBitmap> loadedBMP;

    // The position of the sprite on the canvas.
    // Stored as float so small mouse movements don't have to move the sprite 1 pixel at minimum.
    std::array<float, 2> drawPos = {0,0};
    // The pixel size of a single pegboard
    wxSize m_boardSize = wxSize(50,50);

private:
    // The previous frame's mouse position when left click dragging
    wxPoint prevMouseDragPos = wxPoint();
    
    // The width/height minimum number of boards to fit the sprite, based on sprite dimensions and board dimensions
    std::array<int, 2> m_minimumBoards = {0,0};

    // Stores the IDs of which boards are currently hit by any opaque part of the sprite.
    // ID 0 = top left, ID (minimumBoards[0] * minimumBoards[1]) = bottom right.
    // Left to right then top to bottom.
    std::set<int> m_boardsHit = std::set<int>();

    // Canvas zoom level. 1 = pixel perfect.
    // Ownership in the parent.
    std::shared_ptr<int> m_scale;

    // Flag which triggers the boardsHit thread to recalculate.
    std::atomic_flag m_needsBoardsHitUpdate;

    // Mutex which protects all data members which are required to CalculateBoardsHit.
    std::mutex m_hitMutex;

    /**
     * @brief Thread function to calculate which of the boards are hit by the sprite given its current position.
     * Lifetime matches each unique loaded image. Checks m_needsBoardsHitUpdate as a flag to start calculating.
     * Writes to m_boardsHit;
     * @param _job The Job this function is running on
     */
    void Thread_CalculateBoardsHit(Job* _job);

    // The Job ID for the currently running job calculating which boards are hit
    int m_calculateHitBoardsJobID = -1;

    /**
     * @brief A Job has a state change
     */
    void OnJobStateChanged(const Event_JobStateChanged& _event);
};