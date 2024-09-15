#include <map>
#include "OpenGl.h"
#include <GL/glu.h>
#include <QDebug>
#include <fstream>
#include <QMessageBox>
#include <iostream>

inline QVector3D toQ3d(ColorPoint& val) {
	return QVector3D(val.x, val.y, val.z);
}
inline std::string to_string_f(const float a_value, const int n = 5) {
	std::ostringstream out;
	out << std::fixed << std::setprecision(n) << a_value;
	return out.str();
}
inline std::vector<QString> alignment(std::vector<float> val, int n = 2) {
	int max_length = 0;
	std::vector<QString> ret;
	for (auto i : val) {
		QString msg = QString::number(i, 'f', n);
		if (i >= 0)msg.push_front("+");
		ret.push_back(msg);
		max_length = msg.length() > max_length ? msg.length() : max_length;
	}

	for (auto& i : ret)
		while (i.length() < max_length)i.push_front(" ");

	return ret;
}
inline void WritePcdFileHead(std::fstream& _file, int w, int h, long size, bool is_ascii) {
	_file << "# .PCD v0.7 - Point Cloud Data file format\r\n";
	_file << "VERSION 0.7\r\n";
	_file << "FIELDS x y z rgb\r\n";
	_file << "SIZE 4 4 4 4\r\n";
	_file << "TYPE F F F U\r\n";
	_file << "COUNT 1 1 1 1\r\n";
	_file << "WIDTH " << std::to_string(w) << "\r\n";
	_file << "HEIGHT " << std::to_string(h) << "\r\n";
	_file << "VIEWPOINT 0 0 0 1 0 0 0\r\n";
	_file << "POINTS " << std::to_string(size) << "\r\n";
	_file << "DATA " << (is_ascii ? "ascii" : "binary") << "\r\n";
}
//-----------------------------------------------------------------------------------------------------

OpenGl::OpenGl(QWidget* parent) {
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

	this->setCursor(Qt::ArrowCursor);
	this->setFocusPolicy(Qt::ClickFocus);
	shaderProgram = new QOpenGLShaderProgram();
	shaderProgram_model = new QOpenGLShaderProgram();
	shaderProgram_texture = new QOpenGLShaderProgram();

	vertex_m = "#version 330 core\n\
	layout(location = 0) in vec3 aPos;\n\
	layout(location = 1) in vec3 aColor;\n\
	layout(location = 2) in vec3 Uvw;\n\
	uniform mat4 modelViewProjection;\n\
	uniform float pointSize;\n\
	out vec3 outColor;\n\
	out vec3 outUvw;\n\
	out vec3 outPos;\n\
	void main() {\n\
		gl_Position = modelViewProjection * vec4(aPos, 1.0f);\n\
		gl_PointSize = pointSize; \n\
		outColor = aColor;\n\
		outPos = aPos;\n\
		outUvw = Uvw;\n\
	}";


	fragment_m = "#version 330 core\n\
	uniform float diffuseFactor;\n\
	uniform vec3 lightColor;\n\
	uniform vec3 lightPos;\n\
	in vec3 outColor;\n\
	in vec3 outUvw;\n\
	in vec3 outPos;\n\
	out vec4 FragColor;\n\
	void main(){\n\
		vec3 normol=normalize(outUvw);\n\
		vec3 lightDir = normalize(lightPos - outPos);\n\
		float diffuseStringth = max(dot(normol, lightDir), 0.0);\n\
		vec3 diffuse = 1.0 * diffuseStringth * lightColor;\n\
		FragColor = vec4(diffuse, 1.0) * vec4(outColor, 1.0f);\n\
	}";

	vertex_t = "#version 330 core\n\
	layout(location = 0) in vec3 aPos;\n\
	layout(location = 1) in vec3 aColor;\n\
	layout(location = 2) in vec2 aTexCoord;\n\
	uniform float pointSize;\n\
	uniform mat4 modelViewProjection;\n\
	out vec2 TexCoord;\n\
	void main() {\n\
		gl_Position = modelViewProjection * vec4(aPos, 1.0f);\n\
		gl_PointSize = pointSize; \n\
		TexCoord = aTexCoord;\n\
	}";

	fragment_t = "#version 330 core\n\
	uniform sampler2D ourTexture;\n\
	in vec2 TexCoord;\n\
	out vec4 FragColor;\n\
	void main(){FragColor = texture(ourTexture, TexCoord);}";

	vertex = "#version 330 core\n\
	layout(location = 0) in vec3 aPos;\n\
	layout(location = 1) in vec3 aColor;\n\
	uniform mat4 modelViewProjection;\n\
	uniform float pointSize;\n\
	out vec3 outColor;\n\
	void main() {\n\
		gl_Position = modelViewProjection * vec4(aPos, 1.0f);\n\
		gl_PointSize = pointSize; \n\
		outColor = aColor;\n\
	}";

	fragment = "#version 330 core\n\
	in vec3 outColor;\n\
	out vec4 FragColor;\n\
	void main(){FragColor = vec4(outColor, 1.0f);}";

#ifdef __linux__
	auto v330_to_v300es = [&](QString& f, QString& v) {
		v.replace("#version 330 core\n", "#version 300 es\r");
		f.replace("#version 330 core\n", "#version 300 es\rprecision mediump float;\r");
		v.replace("\n", "\r");
		f.replace("\n", "\r");
	};
	v330_to_v300es(fragment, vertex);
	v330_to_v300es(fragment_m, vertex_m);
	v330_to_v300es(fragment_t, vertex_t);
#endif

	AllCloud[PickupArea].draw_size = 3;
	AllCloud[PickupArea].is_show = true;
	AllCloud[PickupArea].raw_value = true;
	AllCloud[PickupArea].dtype = DLineLoop;

	AllCloud[TrackBall].draw_size = 1;
	AllCloud[TrackBall].dtype = DLine;
	AllCloud[TrackBall].is_show = false;

	AllCloud[TrackBallC].draw_size = 1;
	AllCloud[TrackBallC].is_show = false;
	AllCloud[TrackBallC].dtype = DQuadsStrip;

	AllCloud[PointLine].draw_size = 1;
	AllCloud[PointLine].dtype = DTriangle;
	AllCloud[PointLine].is_show = false;
	AllCloud[PointLine].raw_value = true;

	AllCloud[PointInfo].draw_size = 2;
	AllCloud[PointInfo].dtype = DQuads;
	AllCloud[PointInfo].is_show = false;
	AllCloud[PointInfo].raw_value = true;
	SetPerspective(0.1f, 1000);

	view_raw.lookAt({ 0,0,-1000 }, { 0,0,1000 }, { 0,-1,0 });
}

OpenGl::~OpenGl() {
	glDeleteVertexArrays(1, &SVAO);
	glDeleteBuffers(1, &SVBO);

	shaderProgram_texture->release();
	delete shaderProgram_texture;

	shaderProgram_model->release();
	delete shaderProgram_model;

	shaderProgram->release();
	delete shaderProgram;
}

void OpenGl::initializeGL() {
	initializeOpenGLFunctions();
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex);
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment);
	shaderProgram_model->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_m);
	shaderProgram_model->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_m);
	shaderProgram_texture->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_t);
	shaderProgram_texture->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_t);

	glGenVertexArrays(1, &SVAO);
	glBindVertexArray(SVAO);
	glGenBuffers(1, &SVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SVBO);

	auto bindEnable = [&](QOpenGLShaderProgram* shader, std::vector<int>param) {
		shader->link();
		shader->bind();
		for (auto i : param)glEnable(i);
	};
	bindEnable(shaderProgram_model, { GL_DEPTH_TEST });
	bindEnable(shaderProgram_texture, { GL_DEPTH_TEST });
	bindEnable(shaderProgram, { GL_VERTEX_PROGRAM_POINT_SIZE,GL_DEPTH_TEST });
}

void OpenGl::UpdateUndefineValue() {
	if (AllCloud[TrackBall].is_show) {
		std::vector<ColorPoint> cloud, cloud_c;
		std::vector<float>sin_r, cos_r;
		float half_dis = (cameraEye.distanceToPoint(cameraCenter) * trackball_size_ratio) / 2.f;

		for (float i = -3.14f; i < 3.14f; i += 0.1f) {
			sin_r.push_back(half_dis * sin(i));
			cos_r.push_back(half_dis * cos(i));
		}

		int sin_S = (int)sin_r.size() - 1;
		cloud.push_back({ -half_dis * 0.8f,0,0,1,0,0 });
		cloud.push_back({ half_dis * 0.8f,0,0,1,0,0 });
		for (int i = 1; i <= sin_S; i++) {
			cloud.push_back({ 0,sin_r[i - 1],cos_r[i - 1],1,0,0 });
			cloud.push_back({ 0,sin_r[i],cos_r[i],1,0,0 });
		}
		cloud.push_back({ 0,sin_r[sin_S],cos_r[sin_S],1,0,0 });
		cloud.push_back({ 0,sin_r[0],cos_r[0],1,0,0 });

		cloud.push_back({ 0,-half_dis * 0.8f,0,0,1,0 });
		cloud.push_back({ 0,half_dis * 0.8f,0,0,1,0 });
		for (int i = 1; i <= sin_S; i++) {
			cloud.push_back({ sin_r[i - 1],0,cos_r[i - 1],0,1,0 });
			cloud.push_back({ sin_r[i],0,cos_r[i],0,1,0 });
		}
		cloud.push_back({ sin_r[sin_S],0,cos_r[sin_S],0,1,0 });
		cloud.push_back({ sin_r[0],0,cos_r[0],0,1,0 });

		cloud.push_back({ 0,0,-half_dis * 0.8f,0,0,1 });
		cloud.push_back({ 0,0,half_dis * 0.8f,0,0,1 });
		for (int i = 1; i <= sin_S; i++) {
			cloud.push_back({ cos_r[i - 1],sin_r[i - 1],0,0,0,1 });
			cloud.push_back({ cos_r[i],sin_r[i],0,0,0,1 });
		}
		cloud.push_back({ cos_r[sin_S],sin_r[sin_S],0,0,0,1 });
		cloud.push_back({ cos_r[0],sin_r[0],0,0,0,1 });

		int slices = 30, stacks = 30;
		float radius = half_dis * 0.05f;
		float r(0.047f), g(0.596f), b(0.862f);

		for (int i = 0; i < slices; ++i) {
			float theta1 = i * 2 * M_PI / slices;
			float theta2 = (i + 1) * 2 * M_PI / slices;

			for (int j = 0; j <= stacks; ++j) {
				float phi = j * M_PI / stacks;

				float x1 = radius * sin(theta1) * cos(phi);
				float y1 = radius * sin(theta1) * sin(phi);
				float z1 = radius * cos(theta1);

				float x2 = radius * sin(theta2) * cos(phi);
				float y2 = radius * sin(theta2) * sin(phi);
				float z2 = radius * cos(theta2);

				r += 0.1f;
				r = r > 1 ? r - (int)r : r;
				cloud_c.push_back({ x1, y1, z1,r,g,b });
				cloud_c.push_back({ x2, y2, z2,r,g,b });
			}
			g += 0.01f;
			g = g > 1 ? g - (int)g : g;
		}

		for (auto& i : cloud_c)i += cameraCenter;
		for (auto& i : cloud)i += cameraCenter;
		AllCloud[TrackBallC].cloud = cloud_c;
		AllCloud[TrackBall].cloud = cloud;
	}
	if (AllCloud[PointInfo].is_show) {
		int ds = 3;
		int fs = 20;
		int front = 4;
		int dfs = ds + fs;
		float front_size = 0.5;
		int rows = ds * 8 + fs * 4, cols = 50 + 90 * pick_index;
		auto img_ = cv::Mat(rows, cols, CV_8UC3, cv::Scalar(12, 152, 220));

		float wid = width() / 2.f, hei = height() / 2.f;
		float _w = (10 - wid) / wid, _h = (10 - hei) / hei;
		float b_w = (cols - wid) / wid, b_h = (rows - hei) / hei;

		QVector4D pt[4];
		pt[0] = projection.inverted() * QVector4D(_w, _h, -1, 1);
		pt[1] = projection.inverted() * QVector4D(b_w, _h, -1, 1);
		pt[2] = projection.inverted() * QVector4D(_w, b_h, -1, 1);
		pt[3] = projection.inverted() * QVector4D(b_w, b_h, -1, 1);

		for (int i = 0; i < 4; i++) {
			pt[i] = pt[i] * view_raw.inverted();
			pt[i].setX(pt[i].x() * pt[i].w());
			pt[i].setY(pt[i].y() * pt[i].w());
		}

		AllCloud[PointInfo].texture.clear();
		AllCloud[PointInfo].texture.push_back({ pt[3].x(),pt[3].y(),pt[3].z(),1,1,1,1,0 });
		AllCloud[PointInfo].texture.push_back({ pt[2].x(),pt[2].y(),pt[2].z(),1,1,1,0,0 });
		AllCloud[PointInfo].texture.push_back({ pt[0].x(),pt[0].y(),pt[0].z(),1,1,1,0,1 });
		AllCloud[PointInfo].texture.push_back({ pt[1].x(),pt[1].y(),pt[1].z(),1,1,1,1,1 });

		for (int i = 0; i < cols; i++) {
			for (int j = 0; j < rows; j++) {
				if (j < 2 || i < 2)img_.at<cv::Vec3b>(j, i) = { 255,0,0 };
				if (i - cols > -3)img_.at<cv::Vec3b>(j, i) = { 255,0,0 };
				if (j - rows > -3)img_.at<cv::Vec3b>(j, i) = { 255,0,0 };
			}
		}

		QVector3D pt1{ 0,0,0 }, pt2{ 0,0,0 };
		auto& cloud = AllCloud[pick_name].cloud;
		if (choose_point_1 < cloud.size()) pt1 = toQ3d(cloud[choose_point_1]);
		if (choose_point_2 < cloud.size()) pt2 = toQ3d(cloud[choose_point_2]);

		QString d, x, y, z;
		if (pick_index == 1) {
			auto val = alignment({ pt1.x(),pt1.y(),pt1.z() });
			d = QString("Point #%1").arg(choose_point_1);
			x = QString("^X %1").arg(val[0]);
			y = QString("^Y %1").arg(val[1]);
			z = QString("^Z %1").arg(val[2]);
		}
		else {
			auto val = alignment({ pt1.x(),pt1.y(),pt1.z() }), val2 = alignment({ pt2.x(),pt2.y(),pt2.z() });
			d = "Distance: " + QString::number(pt1.distanceToPoint(pt2), 'f', 6);
			x = QString("X %1  X %2").arg(val[0]).arg(val2[0]);
			y = QString("Y %1  Y %2").arg(val[1]).arg(val2[1]);
			z = QString("Z %1  Z %2").arg(val[2]).arg(val2[2]);
		}
		cv::putText(img_, d.toStdString(), { 10,dfs }, front, front_size, cv::Scalar(0, 0, 0));
		cv::putText(img_, x.toStdString(), { 10,dfs * 2 }, front, front_size, cv::Scalar(0, 0, 0));
		cv::putText(img_, y.toStdString(), { 10,dfs * 3 }, front, front_size, cv::Scalar(0, 0, 0));
		cv::putText(img_, z.toStdString(), { 10,dfs * 4 }, front, front_size, cv::Scalar(0, 0, 0));
		AllCloud[PointInfo].texture_image = img_;


		auto pt1_pos = (PGC * QVector4D(pt1.x(), pt1.y(), pt1.z(), 1.0f)).toVector3DAffine();
		auto pt2_pos = (PGC * QVector4D(pt2.x(), pt2.y(), pt2.z(), 1.0f)).toVector3DAffine();

		QVector4D pt_l[2];
		pt_l[0] = projection.inverted() * QVector4D(pt1_pos.x(), pt1_pos.y(), -1, 1);
		pt_l[1] = projection.inverted() * QVector4D(pt2_pos.x(), pt2_pos.y(), -1, 1);

		for (int i = 0; i < 2; i++) {
			pt_l[i] = pt_l[i] * view_raw.inverted();
			pt_l[i].setX(pt_l[i].x() * pt_l[i].w());
			pt_l[i].setY(pt_l[i].y() * pt_l[i].w());
		}

		std::vector<ColorPoint> cloud_new;
		if (pick_index >= 1) {
			cloud_new.push_back({ pt[2].x(),pt[2].y(),pt[2].z(),1,0,0 });
			cloud_new.push_back({ pt[3].x(),pt[3].y(),pt[3].z(),0,1,0 });
			cloud_new.push_back({ pt_l[0].x(),pt_l[0].y(),pt_l[0].z(),0.047f,0.596f,0.862f });
		}
		if (pick_index >= 2) {
			cloud_new.push_back({ pt[2].x(),pt[2].y(),pt[2].z(),1,0,0 });
			cloud_new.push_back({ pt[3].x(),pt[3].y(),pt[3].z(),0,1,0 });
			cloud_new.push_back({ pt_l[1].x(),pt_l[1].y(),pt_l[1].z(),0.047f,0.596f,0.862f });
		}
		AllCloud[PointLine].cloud = cloud_new;
	}
}
void OpenGl::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UpdateUndefineValue();

	QMatrix4x4 view;
	view.lookAt(cameraEye, cameraCenter, cameraUp);
	golb_view = view;
	PGC = projection * golb_view;
	glBindVertexArray(SVAO);

	auto set_and_get_type = [&](BaseValue& pt, QOpenGLShaderProgram* shard)->unsigned int {
		if (!pt.raw_value) SetSharderValue(shard, "modelViewProjection", PGC);
		else {
			auto qmat = projection * view_raw;
			SetSharderValue(shard, "modelViewProjection", qmat);
		}

		unsigned int draw_type = GL_POINTS;
		if (pt.dtype == DPoint)SetSharderValue(shard, "pointSize", pt.draw_size), draw_type = GL_POINTS;
		if (pt.dtype == DLineLoop)glLineWidth(pt.draw_size), draw_type = GL_LINE_LOOP;
		if (pt.dtype == DLine)glLineWidth(pt.draw_size), draw_type = GL_LINES;
		if (pt.dtype == DTriangle)glLineWidth(1), draw_type = GL_TRIANGLES;
		if (pt.dtype == DQuadsStrip)draw_type = GL_QUAD_STRIP;
		if (pt.dtype == DPoltgon)draw_type = GL_POLYGON;
		if (pt.dtype == DQuads)draw_type = GL_QUADS;
		return draw_type;
	};

	shaderProgram_texture->bind();
	for (auto i : AllCloud) {
		auto& pt = i.second;
		if (!pt.is_show || pt.texture.empty())continue;

		int cols = pt.texture_image.cols, rows = pt.texture_image.rows, ch = pt.texture_image.channels();
		QOpenGLTexture texture(QImage(pt.texture_image.data, cols, rows, cols * ch, QImage::Format_RGB888));

		//texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
		//texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
		//texture->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);

		texture.bind(0);
		SetSharderValue(shaderProgram_texture, "ourTexture", 0);
		auto draw_type = set_and_get_type(pt, shaderProgram_texture);
		glBufferData(GL_ARRAY_BUFFER, pt.texture.size() * 32, pt.texture.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(0); glEnableVertexAttribArray(1); glEnableVertexAttribArray(2);
		glDrawArrays(draw_type, 0, (GLsizei)pt.texture.size());
	}

	shaderProgram->bind();
	for (auto i : AllCloud) {
		auto& pt = i.second;
		if (!pt.is_show || pt.cloud.empty())continue;
		auto draw_type = set_and_get_type(pt, shaderProgram);
		glBufferData(GL_ARRAY_BUFFER, pt.cloud.size() * 24, pt.cloud.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0); glEnableVertexAttribArray(1);
		glDrawArrays(draw_type, 0, (GLsizei)pt.cloud.size());
	}

	shaderProgram_model->bind();
	SetSharderValue(shaderProgram_model, "lightPos", cameraEye);
	SetSharderValue(shaderProgram_model, "lightColor", lightColor);
	SetSharderValue(shaderProgram_model, "diffuseFactor", diffuseFactor);
	for (auto i : AllCloud) {
		auto& pt = i.second;
		if (!pt.is_show || pt.model.empty())continue;
		auto draw_type = set_and_get_type(pt, shaderProgram_model);
		glBufferData(GL_ARRAY_BUFFER, pt.model.size() * 36, pt.model.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(0); glEnableVertexAttribArray(1); glEnableVertexAttribArray(2);
		glDrawArrays(draw_type, 0, (GLsizei)pt.model.size());
	}

	if (auto_update)update();
}

BaseValue OpenGl::LoadPLYFile(std::string path, std::string name, Color color) {
	std::fstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs.is_open())return BaseValue();
	ifs.ignore(80);

	int num_tris;
	BaseValue stl_flie;
	float val[3], f_val[3];
	stl_flie.dtype = DPoint;
	ifs.read((char*)(&num_tris), 4);

	for (int i = 0; i < num_tris; i++) {
		ifs.read((char*)f_val, 12);

		for (int c = 0; c < 3; c++) {
			ifs.read((char*)val, 12);
			stl_flie.model.push_back({ val[0],val[1],val[2],color.r,color.g,color.b,f_val[0],f_val[1],f_val[2] });
		}

		ifs.ignore(2);
	}
	ifs.close();
	AllCloud[name] = stl_flie;
	return stl_flie;
}
BaseValue OpenGl::LoadPCDFile(std::string path, std::string name, Color color) {
	std::fstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs.is_open())return BaseValue();
	ifs.ignore(80);

	int num_tris;
	BaseValue stl_flie;
	float val[3], f_val[3];
	stl_flie.dtype = DPoint;
	ifs.read((char*)(&num_tris), 4);

	for (int i = 0; i < num_tris; i++) {
		ifs.read((char*)f_val, 12);

		for (int c = 0; c < 3; c++) {
			ifs.read((char*)val, 12);
			stl_flie.model.push_back({ val[0],val[1],val[2],color.r,color.g,color.b,f_val[0],f_val[1],f_val[2] });
		}

		ifs.ignore(2);
	}
	ifs.close();
	AllCloud[name] = stl_flie;
	return stl_flie;
}
BaseValue OpenGl::LoadSTLFile(std::string path, std::string name, Color color) {
	std::fstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs.is_open())return BaseValue();
	ifs.ignore(80);

	int num_tris;
	BaseValue stl_flie;
	float val[3], f_val[3];
	stl_flie.dtype = DTriangle;
	ifs.read((char*)(&num_tris), 4);

	for (int i = 0; i < num_tris; i++) {
		ifs.read((char*)f_val, 12);

		for (int c = 0; c < 3; c++) {
			ifs.read((char*)val, 12);
			stl_flie.model.push_back({ val[0],val[1],val[2],color.r,color.g,color.b,f_val[0],f_val[1],f_val[2] });
		}

		ifs.ignore(2);
	}
	ifs.close();
	AllCloud[name] = stl_flie;
	return AllCloud[name];
}
bool OpenGl::savePCDFileBinary(std::string path, BaseValue& point) {
	std::fstream _file(path, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	if (!_file.is_open())return false;

	WritePcdFileHead(_file, point.width, point.height, (long)point.cloud.size(), false);
	for (auto i : point.cloud) {
		std::string a_p;
		a_p.append((char*)&i.x, 4);
		a_p.append((char*)&i.y, 4);
		a_p.append((char*)&i.z, 4);

		char buf[4]{ (char)(i.b * 255),(char)(i.g * 255),(char)(i.r * 255),0 };
		a_p.append(buf, 4);
		_file << a_p;
	}

	_file.close();
	return true;
}
bool OpenGl::savePCDFileASCII(std::string path, BaseValue& point) {
	std::fstream _file(path, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	if (!_file.is_open())return false;

	WritePcdFileHead(_file, point.width, point.height, (long)point.cloud.size(), true);
	for (auto i : point.cloud) {
		char buf[3];
		unsigned int rgb = 0;
		buf[2] = i.r * 255.f;
		buf[1] = i.g * 255.f;
		buf[0] = i.b * 255.f;
		memcpy(&rgb, buf, 3);
		_file << to_string_f(i.x) << " " << to_string_f(i.y) << " " << (int)i.z << " " << rgb << "\r\n";
	}

	_file.close();
	return true;
}

void OpenGl::SetCloudVisible(std::string name, bool is_show) {
	if (!AllCloud.count(name))return;
	AllCloud[name].is_show = is_show;
}
void OpenGl::SetBaseValueSize(std::string name, int size) {
	if (!AllCloud.count(name))return;
	if (size > 10)size = 10;
	if (size < 1)size = 1;
	AllCloud[name].draw_size = size;
}

void OpenGl::AddBaseValue(const BaseValue& val, std::string name) {
	AllCloud[name] = val;
	SetBaseValueSize(name, val.draw_size);
}
void OpenGl::UpdateCloud(const BaseValue& val, std::string name) {
	AllCloud[name].height = val.height;
	AllCloud[name].width = val.width;
	AllCloud[name].cloud = val.cloud;
	AllCloud[name].model = val.model;
	AllCloud[name].dtype = val.dtype;
}
void OpenGl::RemoveBaseValue(std::string name) {
	if (AllCloud.count(name))AllCloud.erase(name);
}

void OpenGl::TransformAndSet(BaseValue& val, std::string name, QMatrix4x4& transfrom) {
}
void OpenGl::TransformPointCloud(std::string name, QMatrix4x4& transfrom) {
	if (!AllCloud.count(name))return;
}
void OpenGl::TransformAndGet(BaseValue& in_out, QMatrix4x4& transfrom) {
}
void OpenGl::Transform(BaseValue& val, __m128* c) {
}

QString OpenGl::GetHardInfo() {
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	return QString("Maximum nr of vertex attributes supported: %1").arg(nrAttributes);
}

void OpenGl::RotatedViewer(float x, float y) {
	float distance = (cameraEye - cameraCenter).length();
	QVector3D viewDirection = (cameraCenter - cameraEye).normalized();

	QVector3D right = QVector3D::crossProduct(viewDirection, cameraUp).normalized();
	cameraUp = QVector3D::crossProduct(right, viewDirection).normalized();

	QVector3D verticalMovement = cameraUp * -y * NORMAL_SPEED * distance / 1000.f;
	QVector3D horizontalMovement = right * x * NORMAL_SPEED * distance / 1000.f;

	cameraEye = cameraEye + horizontalMovement + verticalMovement;
	cameraEye = cameraCenter - (cameraCenter - cameraEye).normalized() * distance;
}
void OpenGl::TranslationViewer(float x, float y) {
	float distance = (cameraEye - cameraCenter).length();
	QVector3D viewDirection = (cameraCenter - cameraEye).normalized();

	QVector3D right = QVector3D::crossProduct(viewDirection, cameraUp).normalized();
	cameraUp = QVector3D::crossProduct(right, viewDirection).normalized();

	QVector3D horizontalMovement = right * x * NORMAL_SPEED / 2.f * distance / 1000.f;
	QVector3D verticalMovement = cameraUp * -y * NORMAL_SPEED / 2.f * distance / 1000.f;

	cameraEye += horizontalMovement + verticalMovement;
	cameraCenter += horizontalMovement + verticalMovement;
}

void OpenGl::keyPressEvent(QKeyEvent* event) {
	float dis = cameraEye.distanceToPoint(cameraCenter) / 10.f;
	QVector3D val(0.0f, 0.0f, 0.0f), val_r(0.0f, 0.0f, 0.0f);
	if (event->key() == Qt::Key_Y) golbal_rot = !golbal_rot;
	else if (event->key() == Qt::Key_W) val = QVector3D(0.0f, -dis, 0.0f);
	else if (event->key() == Qt::Key_S) val = QVector3D(0.0f, dis, 0.0f);
	else if (event->key() == Qt::Key_A) val = QVector3D(-dis, 0.0f, 0.0f);
	else if (event->key() == Qt::Key_D) val = QVector3D(dis, 0.0f, 0.0f);
	else if (event->key() == Qt::Key_E) val = QVector3D(0.0f, 0.0f, dis);
	else if (event->key() == Qt::Key_Q) val = QVector3D(0.0f, 0.0f, -dis);
	else if (event->key() == Qt::Key_J) val_r = QVector3D(-1.0, 0.0, 0.0);
	else if (event->key() == Qt::Key_L) val_r = QVector3D(1.0, 0.0, 0.0);
	else if (event->key() == Qt::Key_I) val_r = QVector3D(0.0, -1.0, 0.0);
	else if (event->key() == Qt::Key_K) val_r = QVector3D(0.0, 1.0, 0.0);
	else if (event->key() == Qt::Key_U) val_r = QVector3D(0.0, 0.0, -1.0);
	else if (event->key() == Qt::Key_O) val_r = QVector3D(0.0, 0.0, 1.0);
	else if (event->key() == Qt::Key_Shift) roSpeed = NORMAL_SPEED * 2;
	else if (event->key() == Qt::Key_Control) roSpeed = NORMAL_SPEED / NORMAL_SPEED;
	else if (key_cb)key_cb(event, key_cb_user);

	if (val_r.x() || val_r.y() || val_r.z()) {
		QQuaternion qua;
		auto eye = cameraEye - cameraCenter;
		if (!golbal_rot) {
			qua = QQuaternion::fromAxisAndAngle(val_r, roSpeed);
			cameraEye = qua.rotatedVector(eye) + cameraCenter;
		}
		else RotatedViewer(val_r.x() * roSpeed, val_r.y() * roSpeed);
	}
	if (val.x() || val.y() || val.z())cameraEye += val, cameraCenter += val;
}
void OpenGl::keyReleaseEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key_Shift) roSpeed = NORMAL_SPEED;
	else if (event->key() == Qt::Key_Control) roSpeed = NORMAL_SPEED;
}
void OpenGl::wheelEvent(QWheelEvent* event) {
	int add_del = event->delta() < 0 ? 1 : -1;
	float diff_x = (cameraEye.x() - cameraCenter.x()) / 10.f * add_del;
	float diff_y = (cameraEye.y() - cameraCenter.y()) / 10.f * add_del;
	float diff_z = (cameraEye.z() - cameraCenter.z()) / 10.f * add_del;
	cameraEye += { diff_x, diff_y, diff_z };
}

void OpenGl::mousePressEvent(QMouseEvent* event) {
	btpush = true;
	pick_pt = event->pos();
	last_pt = event->globalPos();
	if (event->buttons() & Qt::RightButton)pickbtpush = true;
	AllCloud[TrackBallC].is_show = AllCloud[TrackBall].is_show = trackball_show;
}
void OpenGl::mouseReleaseEvent(QMouseEvent* event) {
	if (!pick_name.empty() && AllCloud.count(pick_name) && pickbtpush) {
		auto& cloud = AllCloud[pick_name].cloud;
		float wid = width() / 2.f, hei = height() / 2.f;
		float _w = event->pos().x() - wid, _h = hei - event->pos().y();

		if (event->pos() == pick_pt) {
			unsigned int now_index = 0, min_val = 65535;
			for (unsigned int i = 0; i < cloud.size(); i++) {
				auto pos_ = (PGC * QVector4D(cloud[i].x, cloud[i].y, cloud[i].z, 1.0f)).toVector3DAffine();
				auto diff_w = pos_.x() * wid - _w, diff_h = pos_.y() * hei - _h;
				auto distance = pow(pow(diff_w, 2) + pow(diff_h, 2), 0.5);
				if (distance < min_val) min_val = distance, now_index = i;
			}
			if (pick_up && now_index < cloud.size()) pick_up(now_index, pick_up_user);
			if (!pick_index)choose_point_1 = now_index, pick_index = 1;
			else if (pick_index == 1)choose_point_2 = now_index, pick_index = 2;
			else pick_index = 0;
		}
		else {
			float r_w = pick_pt.x() - wid;
			float r_h = hei - pick_pt.y();
			float min_w = _w > r_w ? r_w : _w;
			float max_w = _w < r_w ? r_w : _w;
			float min_h = _h > r_h ? r_h : _h;
			float max_h = _h < r_h ? r_h : _h;
			std::vector<unsigned int>all_point;

			for (unsigned int i = 0; i < cloud.size(); i++) {
				auto pos_ = (PGC * QVector4D(cloud[i].x, cloud[i].y, cloud[i].z, 1.0f)).toVector3DAffine();
				auto diff_w = pos_.x() * wid, diff_h = pos_.y() * hei;
				if (diff_w < min_w || diff_w > max_w)continue;
				if (diff_h < min_h || diff_h > max_h)continue;
				all_point.push_back(i);
			}
			if (pick_area) pick_area(all_point, pick_area_user);
		}
		AllCloud[PickupArea].is_show = false;
	}
	pickbtpush = btpush = false;
	last_pt = event->globalPos();
	AllCloud[TrackBallC].is_show = AllCloud[TrackBall].is_show = false;
	AllCloud[PointLine].is_show = AllCloud[PointInfo].is_show = pick_index;
}
void OpenGl::mouseMoveEvent(QMouseEvent* event) {
	if (!btpush)return;

	auto now_pt = event->globalPos();
	auto val_x = last_pt.x() - now_pt.x();
	auto val_y = last_pt.y() - now_pt.y();

	if (event->buttons() & Qt::LeftButton) RotatedViewer(val_x, val_y);
	else if (event->buttons() & Qt::MiddleButton) TranslationViewer(val_x, val_y);
	else if (event->buttons() & Qt::RightButton) {
		float wid = width() / 2.f, hei = height() / 2.f;
		float b_w = (pick_pt.x() - wid) / wid, b_h = (hei - pick_pt.y()) / hei;
		float _w = (event->pos().x() - wid) / wid, _h = (hei - event->pos().y()) / hei;

		QVector4D pt[4];
		pt[0] = projection.inverted() * QVector4D(_w, _h, -1, 1);
		pt[1] = projection.inverted() * QVector4D(b_w, _h, -1, 1);
		pt[2] = projection.inverted() * QVector4D(_w, b_h, -1, 1);
		pt[3] = projection.inverted() * QVector4D(b_w, b_h, -1, 1);

		for (int i = 0; i < 4; i++) {
			pt[i] = pt[i] * view_raw.inverted();
			pt[i].setX(pt[i].x() * pt[i].w());
			pt[i].setY(pt[i].y() * pt[i].w());
		}

		AllCloud[PickupArea].cloud.clear();
		AllCloud[PickupArea].cloud.push_back({ pt[3].x(),pt[3].y(),pt[3].z(),1,0,0 });
		AllCloud[PickupArea].cloud.push_back({ pt[2].x(),pt[2].y(),pt[2].z(),0,1,0 });
		AllCloud[PickupArea].cloud.push_back({ pt[0].x(),pt[0].y(),pt[0].z(),0,0,1 });
		AllCloud[PickupArea].cloud.push_back({ pt[1].x(),pt[1].y(),pt[1].z(),1,1,1 });
		AllCloud[PickupArea].is_show = true;
	}

	last_pt = now_pt;
	this->update();
}


void OpenGl::RegisterPickAreaEvent(PickArea* ptr, void* user_data) {
	pick_area = ptr;
	pick_area_user = user_data;
}
void OpenGl::RegisterPickUpEvent(PickUp* ptr, void* user_data) {
	pick_up = ptr;
	pick_up_user = user_data;
}
void OpenGl::RegisterKeyEvent(KeyCB* callback, void* user_data) {
	key_cb = callback;
	key_cb_user = user_data;
}
void OpenGl::SetPickBaseValueName(std::string name) {
	pick_name = name;
}

void OpenGl::SetPerspective(float near_, float far_) {
	QMatrix4x4 projection_;
	projection_.perspective(45.0, (GLfloat)width() / (GLfloat)height(), near_, far_);
	projection = projection_;
}
void OpenGl::SetLightColor(float r, float g, float b) {
	lightColor = QVector3D{ r,g,b };
}
void OpenGl::SetDiffuseFactor(double val) {
	diffuseFactor = val;
}

void OpenGl::SetCameraPosition(QVector3D pos, QVector3D center, QVector3D up) {
	cameraEye = pos; cameraCenter = center; cameraUp = up.normalized();
}

void OpenGl::SetTrackball(double size_ratio, bool is_show) {
	trackball_show = is_show;
	trackball_size_ratio = size_ratio;
}
void OpenGl::SetSystemCoord(float size, bool is_show, float dsize) {
	BaseValue cloud;
	cloud.dtype = DLine;
	cloud.draw_size = dsize;
	cloud.is_show = is_show;
	cloud.cloud.push_back({ 0,0,0,1,0,0 });
	cloud.cloud.push_back({ size,0,0,1,0,0 });
	cloud.cloud.push_back({ 0,0,0,0,1,0 });
	cloud.cloud.push_back({ 0,size,0,0,1,0 });
	cloud.cloud.push_back({ 0,0,0,0,0,1 });
	cloud.cloud.push_back({ 0,0,size,0,0,1 });
	AllCloud[SystemCoord] = cloud;
}
void OpenGl::SetBackGroundColor(double r, double g, double b, double a) {
	glClearColor(r, g, b, a);
}

void OpenGl::SetSharderValue(QOpenGLShaderProgram* shader, QString name, float value) {
	int loc = shader->uniformLocation(name);
	if (loc >= 0) shader->setUniformValue(loc, value);
}
void OpenGl::SetSharderValue(QOpenGLShaderProgram* shader, QString name, QVector3D& value) {
	int loc = shader->uniformLocation(name);
	if (loc >= 0) shader->setUniformValue(loc, value);
}
void OpenGl::SetSharderValue(QOpenGLShaderProgram* shader, QString name, QMatrix4x4& value) {
	int loc = shader->uniformLocation(name);
	if (loc >= 0) shader->setUniformValue(loc, value);
}
