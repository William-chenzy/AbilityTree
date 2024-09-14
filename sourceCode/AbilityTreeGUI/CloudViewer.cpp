#include "CloudViewer.h"

CloudViewer::CloudViewer() {
	gl_ptr = new OpenGl();

	gl_ptr->SetAutoUpdate(false);
	SetCameraPosition({ 0, 0, 200 }, { 0, 0, 0 }, { -1000, 0, 0 });
	gl_ptr->SetPerspective(0.1f, 2000000);
	gl_ptr->SetSystemCoord(100, false, 2);
	gl_ptr->SetTrackball(0.6, false);
	gl_ptr->SetLightColor(1, 1, 1);
	gl_ptr->SetDiffuseFactor(1);
}
CloudViewer::~CloudViewer() {
	delete gl_ptr;
}
bool CloudViewer::MatToCloudPoint(cv::Mat& img, std::string name, int base_z, int num_z, int space_z) {
	if (img.channels() != 4)return false;
	int half_row = img.rows / 2, half_col = img.cols / 2;
	BaseValue logo;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			auto val = img.at<cv::Vec4b>(i, j);

			if (val[3] == 0)continue;
			ColorPoint _dat{ i - half_row ,j - half_col ,base_z,val[0] / 255.f,val[1] / 255.f,val[2] / 255.f };
			for (int z = 0; z < num_z; z++) {
				_dat.z = base_z + z * space_z;
				logo.cloud.push_back(_dat);
			}
		}
	}
	gl_ptr->UpdateCloud(logo, name);
	UpdataOnce();
	return true;
}
bool CloudViewer::AddNewCloud(const BaseValue& val, const std::string& name) {
	gl_ptr->AddBaseValue(val, name);
	UpdataOnce();
	return true;
}

void CloudViewer::SetCameraPosition(QVector3D pos, QVector3D center, QVector3D up) {
	gl_ptr->SetCameraPosition(pos, center, up);
	UpdataOnce();
}

void CloudViewer::SetVisible(std::string name, bool val) { 
	gl_ptr->SetCloudVisible(name, val);
	UpdataOnce();
}
