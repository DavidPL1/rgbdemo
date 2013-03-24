/**
 * Copyright (C) 2013 ManCTL SARL <contact@manctl.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Nicolas Burrus <nicolas.burrus@manctl.com>
 */

#ifndef GUIMULTIKINECTCONTROLLER_H
#define GUIMULTIKINECTCONTROLLER_H

#include "MultiKinectScanner.h"

#include <ntk/thread/event.h>

#include <ntk/camera/rgbd_frame_recorder.h>

class RawImagesWindow;
class View3DWindow;
class FiltersWindow;

class GuiMultiKinectController : public ntk::AsyncEventListener, public MultiKinectController, public ntk::EventBroadcaster
{
    Q_OBJECT

public:
    struct LocalEventData : public ntk::EventData
    {
        TYPEDEF_THIS(LocalEventData)

        CLONABLE_EVENT_DATA

        enum EventType { None, ScannerInitialized };

        LocalEventData() : event_type (None) {}

        EventType event_type;
        ntk::RGBDImageConstPtr new_image;
        std::vector<ntk::RGBDImagePtr> new_synchronized_images;
        MeshVectorPtr new_synchronized_meshes;
        CalibrationParametersPtr calibration_parameters;
    };
    ntk_ptr_typedefs(LocalEventData)

    public:
        GuiMultiKinectController(MultiKinectScanner* scanner);
    virtual ~GuiMultiKinectController();

public:
    // Accessors for GUI
    const ntk::RGBDImage& lastImage() const { return m_last_image; }
    void setActiveDevice(int device);
    void setRecorderDirectory(const std::string& dir) { m_frame_recorder.setDirectory(dir); }

public:
    void resetCheckboardImages();
    void addCheckboardImage();

public:
    // Callbacks from MultiKinectScanner
    virtual void onScannerInitialized();
    virtual void onNewImage(ntk::RGBDImageConstPtr image);
    virtual void onNewSynchronizedImages(const std::vector<ntk::RGBDImagePtr>& images);
    virtual void onNewSynchronizedMeshes(MeshVectorPtr meshes);

public slots:
    void on_depth_mouse_moved(int x, int y);
    void toggleFilters(bool );
    void toggleView3d(bool );
    void saveLastMeshes();
    void setMergeViews(bool merge_them) { m_merge_views = merge_them; }
    void updateCameraCalibrationToGrabber(const cv::Vec3f& new_t, const cv::Vec3f& new_r);
    void onCameraExtrinsicsChanged(CalibrationParametersPtr params);
    void refineCalibrationWithICP();
    void refineCalibrationWithChessboard();
    void setGrabbing(bool grab);
    void grabOneFrame();
    void setBoundingBox(const ntk::Rect3f& bbox, bool update_gui = false);
    const ntk::Rect3f& boundingBox() const { return m_bounding_box; }

protected:
    // In GUI thread.
    virtual void handleAsyncEvent(ntk::EventListener::Event event);
    virtual void processNewImage(ntk::RGBDImageConstPtr image);
    virtual void processNewSynchronizedImages(const std::vector<ntk::RGBDImagePtr>& images);
    virtual void processNewSynchronizedMeshes(MeshVectorPtr meshes);
    virtual void processNewCalibrationParameters(CalibrationParametersPtr params);

    virtual void handleScannerInitialized();

private:
    RawImagesWindow* m_raw_images_window;
    FiltersWindow* m_filters_window;
    View3DWindow* m_view3d_window;
    ntk::RGBDImage m_last_image;
    std::string m_active_device_serial;
    MeshVectorPtr m_last_mesh_vector;    
    bool m_merge_views;
    EventBroadcaster m_init_broadcaster;
    EventBroadcaster m_synchronized_images_broadcaster;
    EventBroadcaster m_synchronized_meshes_broadcaster;
    bool m_grabbing;
    FrameVectorVectorPtr m_checkerboard_frames;
    ntk::RGBDFrameRecorder m_frame_recorder;
    ntk::Rect3f m_bounding_box;
};

#endif // GUIMULTIKINECTCONTROLLER_H
