#ifndef __GLOBAL_DEFINE__H__
#define __GLOBAL_DEFINE__H__
#pragma once
#include <QDir>
#include <QDebug>
#include "CppGlobalDefine.h"
#include <opencv2/opencv.hpp>

#ifdef __linux__
#include <LinuxDefine.h>
#else
#include <WindowsDefine.h>
#endif

#define NORMAL_COLOR cv::Scalar(94, 78, 71, 0)
#define R_COLOR cv::Scalar(109, 112, 246, 255)
#define G_COLOR cv::Scalar(66, 248, 117, 255)

#define SCROLL_AREA_STYLE "QWidget{background-color: transparent;border:0px solid #272e2e;color: #DDDDDD;}\
QScrollBar:vertical{width:4;background:transparent;padding-top:0px;padding-bottom:0px;}\
QScrollBar::handle:vertical{ background: #575e6e; border-radius:2px; }\
QScrollBar::handle:vertical:hover{ background:#676e7e; border-radius:2px; }\
QScrollBar::sub-line:vertical{ background:transparent; height:0px }\
QScrollBar::add-line:vertical{ background:transparent; height:0px }\
QScrollBar::add-page:vertical{ background:transparent; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px; }\
QScrollBar::sub-page:vertical{ background:transparent; border-top-left-radius: 2px; border-top-right-radius: 2px; }\
QScrollBar:horizontal{ height:4; background:transparent; padding-top:0px; padding-bottom:0px; }\
QScrollBar::handle:horizontal{ background: #575e6e; border-radius:2px; }\
QScrollBar::handle:horizontal:hover{ background: #676e7e; border-radius:2px; }\
QScrollBar::sub-line:horizontal{ background:transparent; height:0px }\
QScrollBar::add-line:horizontal{ background:transparent; height:0px }\
QScrollBar::add-page:horizontal{ background:transparent; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px; }\
QScrollBar::sub-page:horizontal{ background:transparent; border-top-left-radius: 2px; border-top-right-radius: 2px; }"

#define PUSH_BUTTON_STYLE "QPushButton{border-radius: 3px;border:0px solid #474e5e;background-color:transparent;color:#DDDDDD;}\
QPushButton:hover{background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #f4716e, stop:0.5 transparent);}\
QPushButton:pressed{background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #42f875, stop:0.5 transparent);}\
QPushButton::disabled{background-color:#7d7d7d;}"

static cv::Mat DrawRGBBcakGround(int w = 48, int h = 48) {
	cv::Mat img = cv::Mat(h, w, CV_8UC4, cv::Scalar(255, 255, 255, 0));

	auto bk_color = [&](cv::Vec3b f, cv::Vec3b e, int len)->std::list<cv::Vec3b> {
		std::list<cv::Vec3b> result;
		float step_r = (f[0] - e[0]) / len * 1.f;
		float step_g = (f[1] - e[1]) / len * 1.f;
		float step_b = (f[2] - e[2]) / len * 1.f;
		for (; len; len--) {
			f[0] -= step_r, f[1] -= step_g, f[2] -= step_b;
			result.push_back(f);
		}
		return result;
	};

	auto b_step = w * 0.1 + 1;
	auto all_step = (w - b_step * 2) / 2;
	for (int i = 0; i < b_step; i++)
		for (int j = 0; j < h; j++)img.at<cv::Vec4b>(j, i) = cv::Scalar(110, 113, 244, 255);

	auto r_g = bk_color({ 110, 113, 244 }, { 117, 248, 66 }, all_step);
	auto it_r_g = r_g.begin();
	for (int i = b_step; i < w / 2; i++) {
		auto c = *it_r_g++;
		for (int j = 0; j < h; j++)
			img.at<cv::Vec4b>(j, i) = cv::Scalar(c[0], c[1], c[2], 255);
	}

	auto g_b = bk_color({ 117, 248, 66 }, { 252, 223, 30 }, all_step);
	auto it_g_b = g_b.begin();
	for (int i = w / 2; i < w - b_step; i++) {
		auto c = *it_g_b++;
		for (int j = 0; j < h; j++)
			img.at<cv::Vec4b>(j, i) = cv::Scalar(c[0], c[1], c[2], 255);
	}

	for (int i = w - b_step; i < w; i++)
		for (int j = 0; j < h; j++)img.at<cv::Vec4b>(j, i) = cv::Scalar(252, 223, 30, 255);

	return img;
}

static cv::Mat DrawColorBar(bool write = false){
	cv::Mat colorbar(20, 1020, CV_8UC3, cv::Scalar(0));
	for (int i = 0; i < 4; i++) {
		if (i == 0) for (int r = 0; r < 255; r++) colorbar.at<cv::Vec3b>(0, i * 255 + r) = cv::Vec3b(255 - r, r, 0);
		if (i == 1) for (int r = 0; r < 255; r++) colorbar.at<cv::Vec3b>(0, i * 255 + r) = cv::Vec3b(0, 255 - r, r);
		if (i == 2) for (int r = 0; r < 255; r++) colorbar.at<cv::Vec3b>(0, i * 255 + r) = cv::Vec3b(r, 0, 255 - r);
		if (i == 3) for (int r = 0; r < 255; r++) colorbar.at<cv::Vec3b>(0, i * 255 + r) = cv::Vec3b(r, r, r);
	}
	for (int x = 1; x < colorbar.rows; x++) colorbar.row(0).copyTo(colorbar.row(x));
	if (write)cv::imwrite("./res/colorbar.png", colorbar);

	cv::Mat colorch(10, 5, CV_8UC4, cv::Scalar(0));
	for (int i = 0; i < 5; i++) colorch.at<cv::Vec4b>(0, i) = cv::Vec4b(255 * (i % 2), 255 * (i % 2), 255 * (i % 2), 255);
	for (int x = 1; x < colorch.rows; x++) colorch.row(0).copyTo(colorch.row(x));
	for (int i = 0; i < 5; i++) colorch.at<cv::Vec4b>(0, i) = colorch.at<cv::Vec4b>(9, i) = cv::Vec4b(0, 0, 0, 255);
	for (int i = 1; i < 3; i++) colorch.at<cv::Vec4b>(1, i) = colorch.at<cv::Vec4b>(8, i) = cv::Vec4b(0, 0, 0, 255);
	colorch.at<cv::Vec4b>(2, 3) = colorch.at<cv::Vec4b>(7, 3) = cv::Vec4b(0, 0, 0, 255);
	for (int i = 0; i < 10; i++) colorch.at<cv::Vec4b>(i, 3) = cv::Vec4b(0, 0, 0, 0);//中间颜色透明
	if (write)cv::imwrite("./res/colorch.png", colorch);
	return colorbar;
}

static cv::Mat GetToolButtonHoverImg(int r, int c, cv::Scalar color, float scale) {
	int all_step = r * scale;
	int singl_step = 255 / all_step;
	cv::Vec4b val(color[0], color[1], color[2], 255);
	cv::Mat img = cv::Mat(r, c, CV_8UC4, NORMAL_COLOR);
	for (int i = r - 1; i >= all_step; i--) {
		for (int j = 0; j < c; j++) img.at<cv::Vec4b>(i, j) = val;
		val[3] -= singl_step;
	}
	return img;
}

static void DrawCloseImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, R_COLOR, 0.5);
	cv::Mat img_pressed = GetToolButtonHoverImg(24, 24, G_COLOR, 0.5);

	int beg = 4, end = 24, step = 2;
	for (int i = beg; i < end - beg; i++) {
		for (int j = 0; j < step; j++) {
			img.at<cv::Vec4b>(i, i + j) = cv::Vec4b(120, 120, 120, 255);
			img_hover.at<cv::Vec4b>(i, i + j) = cv::Vec4b(120, 120, 120, 255);
			img_pressed.at<cv::Vec4b>(i, i + j) = cv::Vec4b(120, 120, 120, 255);

			img.at<cv::Vec4b>(i, end - i - j) = cv::Vec4b(120, 120, 120, 255);
			img_hover.at<cv::Vec4b>(i, end - i - j) = cv::Vec4b(120, 120, 120, 255);
			img_pressed.at<cv::Vec4b>(i, end - i - j) = cv::Vec4b(120, 120, 120, 255);
		}
	}
	cv::imwrite("./res/close.png", img);
	cv::imwrite("./res/close_hover.png", img_hover);
	cv::imwrite("./res/close_pressed.png", img_pressed);
}

static void DrawMaxImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, R_COLOR, 0.5);
	cv::Mat img_pressed = GetToolButtonHoverImg(24, 24, G_COLOR, 0.5);

	int top = 4, mid = 12, bot = 2, wid = 18, pix = 2;
	for (int i = 3; i < 24; i++) {
		for (int j = 3; j < 21; j++) {
			if (top <= 0 && bot <= 0)continue;
			else if (top <= 0 && mid > 0 && !(abs(j - 3 < pix) || abs(j - 20) < pix))continue;
			img.at<cv::Vec4b>(i, j) = cv::Vec4b(120, 120, 120, 255);
		}
		if (top > 0)top--;
		else if (mid > 0) mid--;
		else bot--;
	}

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (img.at<cv::Vec4b>(i, j)[3] == 0)continue;
			img_hover.at<cv::Vec4b>(i, j) = img.at<cv::Vec4b>(i, j);
			img_pressed.at<cv::Vec4b>(i, j) = img.at<cv::Vec4b>(i, j);
		}
	}

	cv::imwrite("./res/max.png", img);
	cv::imwrite("./res/max_hover.png", img_hover);
	cv::imwrite("./res/max_pressed.png", img_pressed);
}

static void DrawFullImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, R_COLOR, 0.5);
	cv::Mat img_pressed = GetToolButtonHoverImg(24, 24, G_COLOR, 0.5);

	int begin_x = 2, begin_y = 6;
	auto raw_color = NORMAL_COLOR;
	int box_size = 14, offset = 4;
	auto color_vr = cv::Vec4b(raw_color[0], raw_color[1], raw_color[2], 255);

	for (int i = begin_y; i < begin_y + box_size; i++) {
		bool line = i == begin_y || i == begin_y + box_size - 1;
		for (int j = begin_x; j < begin_x + box_size; j++) {
			if (!line && j != begin_x && j != begin_x + box_size - 1)continue;
			img.at<cv::Vec4b>(i, j) = cv::Vec4b(120, 120, 120, 255);

			if (i - offset > begin_y && j + offset < begin_x + box_size)continue;
			img.at<cv::Vec4b>(i - offset, j + offset) = cv::Vec4b(120, 120, 120, 255);
		}
	}

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (img.at<cv::Vec4b>(i, j)[3] == 0)continue;
			img_hover.at<cv::Vec4b>(i, j) = img.at<cv::Vec4b>(i, j);
			img_pressed.at<cv::Vec4b>(i, j) = img.at<cv::Vec4b>(i, j);
		}
	}
	cv::imwrite("./res/full.png", img);
	cv::imwrite("./res/full_hover.png", img_hover);
	cv::imwrite("./res/full_pressed.png", img_pressed);
}

static void DrawMinImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, R_COLOR, 0.5);
	cv::Mat img_pressed = GetToolButtonHoverImg(24, 24, G_COLOR, 0.5);

	int width = 18, height = 6;
	int begin_cols = (24 - width) / 2;
	int begin_rows = (24 - height) / 2;
	for (int i = begin_rows; i < begin_rows + height; i++, height--) {
		for (int j = begin_cols; j < begin_cols + width; j++) {
			img.at<cv::Vec4b>(i, j) = cv::Vec4b(120, 120, 120, 255);
			img_hover.at<cv::Vec4b>(i, j) = cv::Vec4b(120, 120, 120, 255);
			img_pressed.at<cv::Vec4b>(i, j) = cv::Vec4b(120, 120, 120, 255);
		}
	}
	cv::imwrite("./res/min.png", img);
	cv::imwrite("./res/min_hover.png", img_hover);
	cv::imwrite("./res/min_pressed.png", img_pressed);
}

static void DrawHelpImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, R_COLOR, 0.5);
	cv::Mat img_pressed = GetToolButtonHoverImg(24, 24, G_COLOR, 0.5);

	cv::circle(img, { 12,12 }, 10, { 120, 120, 120, 255 }, -1, 15);
	cv::putText(img, "?", { 4,20 }, 5, 1.2, { 94, 78, 71, 0 }, 2, 15);

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (img.at<cv::Vec4b>(i, j)[3] == 0)continue;
			img_hover.at<cv::Vec4b>(i, j) = img.at<cv::Vec4b>(i, j);
			img_pressed.at<cv::Vec4b>(i, j) = img.at<cv::Vec4b>(i, j);
		}
	}

	cv::imwrite("./res/help.png", img);
	cv::imwrite("./res/help_hover.png", img_hover);
	cv::imwrite("./res/help_pressed.png", img_pressed);
}

static void DrawSettingImg() {
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	cv::Mat img_hover = GetToolButtonHoverImg(48, 48, R_COLOR, 0.5);
	cv::Mat img_pressed = GetToolButtonHoverImg(48, 48, G_COLOR, 0.5);

	cv::circle(img, { 24,24 }, 20, { 120,120,120,255 }, -1, 120);

	int m_radius = 14;
	for (int i = 0; i < 360; i += 45) {
		float angle = i * PI_U / 180.f;
		int x = m_radius * cos(angle) - m_radius * sin(angle) + 24;
		int y = m_radius * sin(angle) + m_radius * cos(angle) + 24;
		cv::circle(img, { x,y }, 3, { 120,120,120,0 }, -1, 120);
	}
	cv::circle(img, { 24,24 }, 8, { 120,120,120,0 }, -1, 120);

	for (int i = 0; i < 48; i++)
		for (int j = 0; j < 48; j++) {
			if (!img.at<cv::Vec4b>(i, j)[3])continue;
			auto val = img.at<cv::Vec4b>(i, j);
			img_hover.at<cv::Vec4b>(i, j) = val;
			img_pressed.at<cv::Vec4b>(i, j) = val;
		}

	cv::imwrite("./res/setting.png", img);
	cv::imwrite("./res/setting_hover.png", img_hover);
	cv::imwrite("./res/setting_pressed.png", img_pressed);
}

static void DrawSerchImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC4, cv::Scalar(94, 78, 71, 0));
	cv::circle(img, { 10,10 }, 5, { 120, 120, 120 , 255 }, 1, 25);

	for (int i = 17; i < 24; i++)
		for (int j = 0; j < 3; j++) {
			img.at<cv::Vec4b>(i - 2, i + j) = { 120, 120, 120, 255 };
		}

	cv::imwrite("./res/serch.png", img);
}

static void DrawFrontIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	int line2_height = 10, line3_width = 36;
	int line_wid = 3, height = 20, width = 32, r_begin = 8, c_begin = 8;
	int null_r = r_begin + line_wid, null_r_end = r_begin + height - line_wid;
	int null_c = c_begin + line_wid, null_c_end = c_begin + width - line_wid;

	for (int i = r_begin; i < r_begin + height; i++) {
		for (int j = c_begin; j < c_begin + width; j++) {
			if (i >= null_r && i < null_r_end && j >= null_c && j < null_c_end)continue;
			img.at<cv::Vec4b>(i, j) = { 170, 170, 170, 255 };
		}
	}

	int line2_hb = r_begin + height, line2_wb = c_begin + width / 2 - line_wid / 2;
	for (int i = line2_hb; i < line2_hb + line2_height; i++) {
		for (int j = line2_wb; j < line2_wb + line_wid; j++) {
			if (i >= null_r && i < null_r_end && j >= null_c && j < null_c_end)continue;
			img.at<cv::Vec4b>(i, j) = { 170, 170, 170, 255 };
		}
	}

	int line3_hb = line2_hb + line2_height, line3_wb = c_begin + width / 2 - line3_width / 2;
	for (int i = line3_hb; i < line3_hb + line_wid; i++) {
		for (int j = line3_wb; j < line3_wb + line3_width - 1; j++) {
			if (i >= null_r && i < null_r_end && j >= null_c && j < null_c_end)continue;
			img.at<cv::Vec4b>(i, j) = { 170, 170, 170, 255 };
		}
	}

	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0];
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/FrontIcon.png", img);
	cv::imwrite("./res/FrontIcon_bk.png", img_bk);
}

static void DrawNetworkIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	int base_abgle = 15, base_len = 24;
	float circle_num = 3, circle_space = 3.6, radius = 5;

	float base_angle = base_abgle * PI_U / 180.f;
	for (int i = 0; i < base_len; i++) {
		float x = i * cos(base_angle) + 24;
		float y = i * sin(base_angle) + 24;
		float y2 = i * sin(-base_angle) + 24;
		img.at<cv::Vec4b>(x, y) = { 170, 170, 170, 255 };
		img.at<cv::Vec4b>(x, y2) = { 170, 170, 170, 255 };

		if (base_len - i <= base_len / 2) {
			int half = (y - y2) / 5;
			for (int j = y2; j < y2 + half; j++) img.at<cv::Vec4b>(x, j) = { 170, 170, 170, 255 };
			for (int j = y - half; j < y; j++) img.at<cv::Vec4b>(x, j) = { 170, 170, 170, 255 };
		}
		else for (int j = y2; j < y; j++) img.at<cv::Vec4b>(x, j) = { 170, 170, 170, 255 };
	}

	for (; circle_num; circle_num--) {
		for (int i = 0; i < 360; i++) {
			if (abs(i - 315) <= base_abgle * 2)continue;
			float angle = i * PI_U / 180.f;
			float x = radius * cos(angle) - radius * sin(angle) + 24;
			float y = radius * sin(angle) + radius * cos(angle) + 24;
			img.at<cv::Vec4b>(x, y) = { 170, 170, 170, 255 };
		}
		radius += circle_space;
	}


	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0];
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/NetworkIcon.png", img);
	cv::imwrite("./res/NetworkIcon_bk.png", img_bk);
}

static void DrawAssembleIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	cv::putText(img, "1101", cv::Point{ 6,18 }, 1, 0.9, { 170, 170, 170, 255 }, 2, 25);
	cv::putText(img, "1010", cv::Point{ 6,36 }, 1, 0.9, { 170, 170, 170, 255 }, 2, 25);

	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0] > 100;
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/AssembleIcon.png", img);
	cv::imwrite("./res/AssembleIcon_bk.png", img_bk);
}

static void DrawHackerIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	int hat_line_height = 3, hat_line_spce = 5, hat_height = 20;
	int hat_top_height = 5, hat_bottom_height = 2;
	int hat_top_width = 12, hat_bottom_width = 36;
	int line_width = 2, begin_rows = 12, line_offset = 5;

	int top_offset = (48 - hat_top_width) / 2;
	for (int i = begin_rows; i < begin_rows + hat_top_height; i++) {
		for (int j = 0; j < hat_top_width; j++)
			img.at<cv::Vec4b>(i, j + top_offset) = { 170, 170, 170, 255 };
	}

	int bottom_offset = (48 - hat_bottom_width) / 2;
	for (int i = begin_rows + hat_height; i < begin_rows + hat_height + hat_bottom_height; i++) {
		for (int j = 0; j < hat_bottom_width; j++)
			img.at<cv::Vec4b>(i, j + bottom_offset) = { 170, 170, 170, 255 };
	}

	float line_being_cols = top_offset;
	float step_x = (top_offset - bottom_offset - line_offset) * 1.f / hat_height * 1.f;
	for (int i = begin_rows; i < begin_rows + hat_height; i++) {
		for (int j = 0; j < line_width; j++)
			img.at<cv::Vec4b>(i, line_being_cols + j) = { 170, 170, 170, 255 };
		line_being_cols -= step_x;
	}
	line_being_cols = top_offset - 1;
	for (int i = begin_rows; i < begin_rows + hat_height; i++) {
		for (int j = 0; j < line_width; j++)
			img.at<cv::Vec4b>(i, line_being_cols + j + hat_top_width) = { 170, 170, 170, 255 };
		line_being_cols += step_x;
	}

	int line_begin_rows = begin_rows + hat_height - hat_line_spce;
	for (int i = line_begin_rows; i > line_begin_rows - hat_line_height; i--) {
		bool left_s = false, right_s = false;
		for (int j = 0; j <= 24; j++) {
			if (!left_s)left_s = img.at<cv::Vec4b>(i, j)[0];
			if (left_s)img.at<cv::Vec4b>(i, j) = { 170, 170, 170, 255 };
			if (!left_s)left_s = img.at<cv::Vec4b>(i, 48 - j)[0];
			if (left_s)img.at<cv::Vec4b>(i, 48 - j) = { 170, 170, 170, 255 };
		}
	}



	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0] > 100;
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/HackerIcon.png", img);
	cv::imwrite("./res/HackerIcon_bk.png", img_bk);
}

static void DrawAlgorithmIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	int radius = 3, space_v = 24, space_h = 10;

	int beg_x = (48 - 3 * space_h) / 2;
	int beg_y = (48 - 1 * space_v) / 2;
	std::vector<cv::Point>left_, mid_, mid_2, right;
	left_.push_back({ beg_x,24 });

	mid_.push_back({ beg_x + space_h,beg_y });
	mid_.push_back({ beg_x + space_h,beg_y + space_v });;

	mid_2.push_back({ beg_x + space_h * 2,beg_y });
	mid_2.push_back({ beg_x + space_h * 2,beg_y + space_v });

	right.push_back({ beg_x + space_h * 3,24 });

	for (auto i : left_)cv::circle(img, i, radius, { 170,170,170,255 }, 1, 25);
	for (auto i : mid_)cv::circle(img, i, radius, { 170,170,170,255 }, -1, 25);
	for (auto i : mid_2)cv::circle(img, i, radius, { 170,170,170,255 }, -1, 25);
	for (auto i : right)cv::circle(img, i, radius, { 170,170,170,255 }, 1, 25);

	auto add_line = [&](std::vector<cv::Point>& f, std::vector<cv::Point>& s) {
		for (auto i : f)
			for (auto j : s) cv::line(img, i, j, { 170,170,170,255 }, 1, 25);
	};
	add_line(left_, mid_);
	add_line(left_, mid_2);
	add_line(right, mid_);
	add_line(right, mid_2);
	add_line(mid_, mid_2);


	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0] > 100;
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/AlgorithmIcon.png", img);
	cv::imwrite("./res/AlgorithmIcon_bk.png", img_bk);
}

static void DrawDeveloperIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	cv::putText(img, "</>", cv::Point{ 9,28 }, 1, 1, { 170, 170, 170, 255 }, 2, 50);

	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0] > 80;
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/DeveloperIcon.png", img);
	cv::imwrite("./res/DeveloperIcon_bk.png", img_bk);
}

static void DrawOtherIcon() {
	cv::Mat img_bk, img;
	img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	img_bk = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	cv::rectangle(img, cv::Rect(8, 8, 12, 12), { 170,170,170,255 }, -1, 50);
	cv::circle(img, { 33,14 }, 6, { 170,170,170,255 }, 1, 50);

	cv::rectangle(img, cv::Rect(8, 28, 12, 12), { 170,170,170,255 }, 1, 50);
	cv::circle(img, { 33,33 }, 6, { 170,170,170,255 }, -1, 50);

	for (int i = 0; i < 47; i++) {
		for (int j = 0; j < 47; j++) {
			bool is_color = img.at<cv::Vec4b>(i, j)[0] > 80;
			img_bk.at<cv::Vec4b>(i, j) = { 170, 170, 170, (uchar)(is_color ? 0 : 255) };
		}
	}

	cv::imwrite("./res/OtherIcon.png", img);
	cv::imwrite("./res/OtherIcon_bk.png", img_bk);
}

static void DrawBrokenIcon() {
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	cv::circle(img, { 24,24 }, 20, { 170,170,170,255 }, -1, 50);
	cv::putText(img, "!", cv::Point{ 17,39 }, 1, 3, { 170, 170, 170, 0 }, 2, 50);

	cv::imwrite("./res/BrokenIcon.png", img);
}

static void DrawExpandImg() {
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(255, 255, 255, 0));
	cv::putText(img, "+", cv::Point{ 6,39 }, 1, 3, { 170, 170, 170, 255 }, 2, 50);
	cv::imwrite("./res/expand.png", img);
}

static void DrawRetractImg() {
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(255, 255, 255, 0));
	cv::putText(img, "-", cv::Point{ 6,39 }, 1, 3, { 170, 170, 170, 255 }, 2, 50);
	cv::imwrite("./res/retract.png", img);
}

static cv::Mat DrawMouseCircleImg(bool save = true) {
	cv::Mat bk = DrawRGBBcakGround();
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	cv::circle(img, { 24,24 }, 20, { 50,50,50,255 }, -1, 120);

	for (int i = 0; i < 48; i++) {
		for (int j = 0; j < 48; j++) {
			if (img.at<cv::Vec4b>(i, j)[0] == 0)continue;
			img.at<cv::Vec4b>(i, j) = bk.at<cv::Vec4b>(i, j);
		}
	}
	cv::circle(img, { 24,24 }, 12, { 255,255,255,20 }, 3, 120);
	if (save)cv::imwrite("./res/mouseCircle.png", img);
	return img;
}
static void DrawMouseLeftImg() {
	cv::Mat img = DrawMouseCircleImg(false);

	for (int i = 0; i < 48; i++) {
		for (int j = 0; j < 24; j++) {
			if (img.at<cv::Vec4b>(i, j)[3] == 20)break;
			if (img.at<cv::Vec4b>(i, j)[3] != 255)continue;

			img.at<cv::Vec4b>(i, j) = { 255,255,255,255 };
		}
	}

	cv::imwrite("./res/mouseLeft.png", img);
}
static void DrawMouseRightImg() {
	cv::Mat img = DrawMouseCircleImg(false);

	for (int i = 0; i < 48; i++) {
		for (int j = 47; j > 24; j--) {
			if (img.at<cv::Vec4b>(i, j)[3] == 20)break;
			if (img.at<cv::Vec4b>(i, j)[3] != 255)continue;

			img.at<cv::Vec4b>(i, j) = { 255,255,255,255 };
		}
	}

	cv::imwrite("./res/mouseRight.png", img);
}
static void DrawMouseMiddleImg() {
	cv::Mat img = DrawMouseCircleImg(false);

	cv::circle(img, { 24,24 }, 9, { 255,255,255,255 }, -1, 120);
	cv::imwrite("./res/mouseMiddle.png", img);
}

static void DrawDownImg() {
	cv::Mat img = cv::Mat(12, 48, CV_8UC4, NORMAL_COLOR);

	cv::line(img, { 0,15 }, { 12,0 }, { 255,255,255,255 }, 1, 120);
	cv::line(img, { 12,0 }, { 36,0 }, { 255,255,255,255 }, 1, 120);
	cv::line(img, { 36,0 }, { 48,15 }, { 255,255,255,255 }, 1, 120);

	cv::line(img, { 18,4 }, { 24,8 }, { 255,255,255,255 }, 1, 120);
	cv::line(img, { 24,8 }, { 30,4 }, { 255,255,255,255 }, 1, 120);

	cv::imwrite("./res/down_img.png", img);
}
static void DrawUpImg() {
	cv::Mat img = cv::Mat(12, 48, CV_8UC4, NORMAL_COLOR);

	cv::line(img, { 0,15 }, { 12,0 }, { 255,255,255,255 }, 1, 120);
	cv::line(img, { 12,0 }, { 36,0 }, { 255,255,255,255 }, 1, 120);
	cv::line(img, { 36,0 }, { 48,15 }, { 255,255,255,255 }, 1, 120);

	cv::line(img, { 18,8 }, { 24,4 }, { 255,255,255,255 }, 1, 120);
	cv::line(img, { 24,4 }, { 30,8 }, { 255,255,255,255 }, 1, 120);

	cv::imwrite("./res/up_img.png", img);
}
static void DrawRightImg() {
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, NORMAL_COLOR);

	cv::line(img, { 18,18 }, { 30,24 }, { 255,255,255,255 }, 2, 120);
	cv::line(img, { 30,24 }, { 18,30 }, { 255,255,255,255 }, 2, 120);

	cv::imwrite("./res/right_img.png", img);
}
static void DrawLeftImg() {
	cv::Mat img = cv::Mat(48, 48, CV_8UC4, NORMAL_COLOR);

	cv::line(img, { 30,18 }, { 18,24 }, { 255,255,255,255 }, 2, 120);
	cv::line(img, { 18,24 }, { 30,30 }, { 255,255,255,255 }, 2, 120);

	cv::imwrite("./res/left_img.png", img);
}
static void DrawImageFullImg() {
	bool img_mask[24][24]{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_full.png", img);
	cv::imwrite("./res/image_full_hover.png", img_hover);
	cv::imwrite("./res/image_full_pressed.png", img_pressed);
}
static void DrawImageFullCancelImg() {
	bool img_mask[24][24]{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,
		0,0,0,0,0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,0,0,0,0,
		0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,
		0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,
		0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_full_cancel.png", img);
	cv::imwrite("./res/image_full_cancel_hover.png", img_hover);
	cv::imwrite("./res/image_full_cancel_pressed.png", img_pressed);
}
static void DrawImageMaxImg() {
	bool img_mask[24][24]{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_max.png", img);
	cv::imwrite("./res/image_max_hover.png", img_hover);
	cv::imwrite("./res/image_max_pressed.png", img_pressed);
}
static void DrawImageMaxCancelImg() {
	bool img_mask[24][24]{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_max_cancel.png", img);
	cv::imwrite("./res/image_max_cancel_hover.png", img_hover);
	cv::imwrite("./res/image_max_cancel_pressed.png", img_pressed);
}
static void DrawImageBiggerImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,1,1,1,1,1,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_bigger.png", img);
	cv::imwrite("./res/image_bigger_hover.png", img_hover);
	cv::imwrite("./res/image_bigger_pressed.png", img_pressed);
}
static void DrawImageSmallImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,1,1,1,1,1,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_small.png", img);
	cv::imwrite("./res/image_small_hover.png", img_hover);
	cv::imwrite("./res/image_small_pressed.png", img_pressed);
}
static void DrawImagePrevImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_prev.png", img);
	cv::imwrite("./res/image_prev_hover.png", img_hover);
	cv::imwrite("./res/image_prev_pressed.png", img_pressed);
}
static void DrawImageNextImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_next.png", img);
	cv::imwrite("./res/image_next_hover.png", img_hover);
	cv::imwrite("./res/image_next_pressed.png", img_pressed);
}
static void DrawImageLeftRotateImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_left_rotate.png", img);
	cv::imwrite("./res/image_left_rotate_hover.png", img_hover);
	cv::imwrite("./res/image_left_rotate_pressed.png", img_pressed);
}
static void DrawImageRightRotateImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_right_rotate.png", img);
	cv::imwrite("./res/image_right_rotate_hover.png", img_hover);
	cv::imwrite("./res/image_right_rotate_pressed.png", img_pressed);
}
static void DrawImageDeleteImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,   0,0,0,0,
		0,0,0,0,   0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_delete.png", img);
	cv::imwrite("./res/image_delete_hover.png", img_hover);
	cv::imwrite("./res/image_delete_pressed.png", img_pressed);
}
static void DrawImageInfoImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_info.png", img);
	cv::imwrite("./res/image_info_hover.png", img_hover);
	cv::imwrite("./res/image_info_pressed.png", img_pressed);
}
static void DrawImageInfoCancelImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/image_info_cancel.png", img);
	cv::imwrite("./res/image_info_cancel_hover.png", img_hover);
	cv::imwrite("./res/image_info_cancel_pressed.png", img_pressed);
}

static void DrawOpenDirImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/open_dir.png", img);
	cv::imwrite("./res/open_dir_hover.png", img_hover);
	cv::imwrite("./res/open_dir_pressed.png", img_pressed);
}
static void DrawSaveAsImg() {
	bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,
		0,0,0,0,   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
	};

	cv::Mat img = cv::Mat(24, 24, CV_8UC4, NORMAL_COLOR);
	cv::Mat img_hover = cv::Mat(24, 24, CV_8UC4, { 255,255,255,36 });
	cv::Mat img_pressed = cv::Mat(24, 24, CV_8UC4, { 255,255,255,48 });

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (!img_mask[i][j])continue;
			img.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_hover.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
			img_pressed.at<cv::Vec4b>(i, j) = { 225, 225, 225, 255 };
		}
	}

	cv::imwrite("./res/save_as.png", img);
	cv::imwrite("./res/save_as_hover.png", img_hover);
	cv::imwrite("./res/save_as_pressed.png", img_pressed);
}

static void InitDir() {
	QDir dir;
	dir.mkpath("./res/");
	dir.mkpath("./conf/");
	dir.mkpath("./temp/");
}
static void DrawAllImg() {
	InitDir();

	DrawDeveloperIcon();
	DrawAlgorithmIcon();
	DrawAssembleIcon();
	DrawNetworkIcon();
	DrawSettingImg();
	DrawBrokenIcon();
	DrawHackerIcon();
	DrawFrontIcon();
	DrawOtherIcon();
	DrawSerchImg();
	DrawCloseImg();
	DrawHelpImg();
	DrawFullImg();
	DrawMinImg();
	DrawMaxImg();
	DrawColorBar();
	DrawExpandImg();
	DrawRetractImg();
	DrawMouseLeftImg();
	DrawMouseRightImg();
	DrawMouseMiddleImg();
	DrawMouseCircleImg();

	DrawUpImg();
	DrawDownImg();
	DrawLeftImg();
	DrawRightImg();
	DrawImageMaxImg();
	DrawImageFullImg();
	DrawImagePrevImg();
	DrawImageNextImg();
	DrawImageInfoImg();
	DrawImageSmallImg();
	DrawImageDeleteImg();
	DrawImageBiggerImg();
	DrawImageMaxCancelImg();
	DrawImageInfoCancelImg();
	DrawImageFullCancelImg();
	DrawImageLeftRotateImg();
	DrawImageRightRotateImg();

	DrawOpenDirImg();
	DrawSaveAsImg();
}


static bool CheckFile(QString name) {
	QFileInfo fileInfo(name);
	return fileInfo.exists();
}
static std::string GetTime() {
	long long mil = GetTimeM % 1000000;
	tm _t = GetTimeT();
	QString tim_ = QString("[%1-%2-%3 %4:%5:%6 %7] ").arg(_t.tm_year + 1900).arg(_t.tm_mon + 1).arg(_t.tm_mday).arg(_t.tm_hour, 2, 10, QLatin1Char('0'))
		.arg(_t.tm_min, 2, 10, QLatin1Char('0')).arg(_t.tm_sec, 2, 10, QLatin1Char('0')).arg(mil, 6, 10, QLatin1Char('0'));
	return tim_.toStdString();
}
static std::string GetTime(timeval tv) {
	time_t nows = tv.tv_sec;
	tm* _t = new tm();
	localtime_s(_t, &nows);
	QString tim_ = QString("%1-%2-%3 %4:%5:%6 %7").arg(_t->tm_year + 1900).arg(_t->tm_mon + 1).arg(_t->tm_mday).arg(_t->tm_hour, 2, 10, QLatin1Char('0'))
		.arg(_t->tm_min, 2, 10, QLatin1Char('0')).arg(_t->tm_sec, 2, 10, QLatin1Char('0')).arg(tv.tv_usec, 6, 10, QLatin1Char('0'));
	return tim_.toStdString();
}


/*bool img_mask[24][24]{
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,

		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,
		0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0
};*/

#endif //__GLOBAL_DEFINE__H__
