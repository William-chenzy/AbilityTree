#ifndef __OPEN_GL_H__
#define __OPEN_GL_H__

#include <map>
#include <QtMath>
#include <string>
#include <QKeyEvent>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QElapsedTimer>
#include <QOpenGLTexture>
#include <opencv2/opencv.hpp>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <GL/glu.h>

#define NORMAL_SPEED 8
#define PointLine "PointLine"
#define PointInfo "PointInfo"
#define TrackBall "TrackBall"
#define TrackBallC "TrackBall_C"
#define PickupArea "PickUp_Area"
#define SystemCoord "System_Coord"
typedef void (KeyCB)(QKeyEvent*, void*);
typedef void (PickUp)(unsigned int, void*);
typedef void (PickArea)(std::vector<unsigned int>, void*);
enum DTYPE{DPoint,DLine,DLineLoop,DQuads,DQuadsStrip,DTriangle,DPoltgon};
struct Color {
	float r, g, b;
};
struct ColorPoint{
	float x, y, z;
	float r, g, b;
	ColorPoint& operator+=(const QVector3D& val) {
		this->x += val.x();
		this->y += val.y();
		this->z += val.z();
		return *this;
	}
};
struct ModelPoint {
	float x, y, z;
	float r, g, b;
	float fx, fy, fz;
	ModelPoint& operator+=(const QVector3D& val) {
		this->x += val.x();
		this->y += val.y();
		this->z += val.z();
		return *this;
	}
};
struct TexturePoint {
	float x, y, z;
	float r, g, b;
	float fx, fy;
	TexturePoint& operator+=(const QVector3D& val) {
		this->x += val.x();
		this->y += val.y();
		this->z += val.z();
		return *this;
	}
};
struct BaseValue {
	std::vector<TexturePoint> texture;
	std::vector<ColorPoint> cloud;
	std::vector<ModelPoint> model;
	bool raw_value = false;
	cv::Mat texture_image;
	DTYPE dtype = DPoint;
	float draw_size = 1;
	bool is_show = true;
	int height = 0;
	int width = 0;

	const BaseValue& operator=(const BaseValue& val) {
		this->cloud = val.cloud;
		this->model = val.model;
		this->draw_size = val.draw_size;
		this->is_show = val.is_show;
		this->height = val.height;
		this->width = val.width;
		this->dtype = val.dtype;
		return *this;
	}
};
class OpenGl : public QOpenGLWidget, protected QOpenGLExtraFunctions {

	Q_OBJECT

public:
	OpenGl(QWidget* parent = 0);
	~OpenGl();

	QString GetHardInfo();
	void SetDiffuseFactor(double val);
	void RemoveBaseValue(std::string name);
	void SetPickBaseValueName(std::string name);
	void SetPerspective(float near_, float far_);
	void SetLightColor(float r, float g, float b);
	void SetBaseValueSize(std::string name, int size);
	void SetTrackball(double size_ratio, bool is_show);
	void SetCloudVisible(std::string name, bool is_show);
	bool savePCDFileASCII(std::string path, BaseValue& point);
	bool savePCDFileBinary(std::string path, BaseValue& point);
	void SetSystemCoord(float size, bool is_show, float dsize);
	void UpdateCloud(const BaseValue& val, const std::string name);
	void AddBaseValue(const BaseValue& val, const std::string name);
	void TransformAndGet(BaseValue& in_out, QMatrix4x4& transfrom);
	void SetBackGroundColor(double r, double g, double b, double a);
	void TransformPointCloud(std::string name, QMatrix4x4& transfrom);
	void SetCameraPosition(QVector3D pos, QVector3D center, QVector3D up);
	void TransformAndSet(BaseValue& val, std::string name, QMatrix4x4& transfrom);

	BaseValue LoadSTLFile(std::string path, std::string name, Color color);
	BaseValue LoadPLYFile(std::string path, std::string name, Color color);
	BaseValue LoadPCDFile(std::string path, std::string name, Color color);

	void RegisterPickAreaEvent(PickArea* ptr, void* user_data);
	void RegisterKeyEvent(KeyCB* callback, void* user_data);
	void RegisterPickUpEvent(PickUp* ptr, void* user_data);
	void SetAutoUpdate(bool val) { auto_update = val; }
	void UpdateOnce() { if (!auto_update)update(); }

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void initializeGL() override;
	void paintGL() override;

private:
	void UpdateUndefineValue();
	void RotatedViewer(float x, float y);
	void TranslationViewer(float x, float y);
	void Transform(BaseValue& val, __m128* c);
	void SetSharderValue(QOpenGLShaderProgram* shader, QString name, float value);
	void SetSharderValue(QOpenGLShaderProgram* shader, QString name, QVector3D& value);
	void SetSharderValue(QOpenGLShaderProgram* shader, QString name, QMatrix4x4& value);

private:
	QString vertex, fragment;
	QString vertex_m, fragment_m;
	QString vertex_t, fragment_t;
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShaderProgram* shaderProgram_model;
	QOpenGLShaderProgram* shaderProgram_texture;
	GLuint SVBO, SVAO, SVBO_m, SVAO_m, SVBO_t, SVAO_t;

	QVector3D cameraUp;
	QVector3D cameraEye;
	QVector3D cameraCenter;
	QMatrix4x4 projection;
	QMatrix4x4 golb_view;
	QMatrix4x4 view_raw;
	QMatrix4x4 PGC;

	std::map<std::string, BaseValue>AllCloud;
	float roSpeed = NORMAL_SPEED;

	QPoint last_pt;
	QPoint pick_pt;
	bool auto_update;
	int pick_index = 0;
	bool btpush = false;
	bool pickbtpush = false;

	bool trackball_show = true;
	float trackball_size_ratio = 0.6f;

	unsigned int choose_point_1;
	unsigned int choose_point_2;

	float diffuseFactor;
	QVector3D lightColor;
	bool golbal_rot = true;

	std::string pick_name;
	KeyCB* key_cb = nullptr;
	void* key_cb_user = nullptr;

	PickUp* pick_up = nullptr;
	void* pick_up_user = nullptr;

	PickArea* pick_area = nullptr;
	void* pick_area_user = nullptr;
};

#endif //__OPEN_GL_H__