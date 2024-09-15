#ifndef __CLOUD_VIEWER_H__
#define __CLOUD_VIEWER_H__

#include "OpenGl.h"
struct CLOUD_STU {
	BaseValue _points;
	bool is_show = true;
};

class CloudViewer : public QObject{

	Q_OBJECT

public:
	CloudViewer();
	~CloudViewer();

public:
	void RemoveBaseValue(std::string name);
	OpenGl * const GlPtr() { return gl_ptr; };
	void UpdataOnce() { gl_ptr->UpdateOnce(); };
	void SetVisible(std::string name, bool val);
	bool AddNewCloud(const BaseValue& val, const std::string& name);
	void SetCameraPosition(QVector3D pos, QVector3D center, QVector3D up);
	bool MatToCloudPoint(cv::Mat& img, std::string name, int base_z, int num_z, int space_z);

private:
	OpenGl* gl_ptr = nullptr;
};

#endif //__CLOUD_VIEWER_H__