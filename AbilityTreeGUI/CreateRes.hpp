#ifndef CREATE_RES_H
#define CREATE_RES_H

#include <qdebug.h>
#include "opencv2/opencv.hpp"
#define NORMAL_COLOR cv::Scalar(94, 78, 71)
#define RED_COLOR cv::Scalar(109, 112, 246)


static cv::Mat GetToolButtonHoverImg(int r,int c, cv::Scalar src, cv::Scalar dst) {
	float rgb_step[3]{ 0,0,0 };
	auto color_vc = cv::Vec3b(dst[0], dst[1], dst[2]);
	auto color_vr = cv::Vec3b(src[0], src[1], src[2]);
	cv::Mat img = cv::Mat(r, c, CV_8UC3, NORMAL_COLOR);
	for (int c = 0; c < 3; c++)rgb_step[c] = (dst[c] - src[c]) / (r / 2.f);
	for (int i = r - 1; i >= r / 2; i--) {
		for (int j = 0; j < c; j++) img.at<cv::Vec3b>(i, j) = color_vc;
		for (int c = 0; c < 3; c++) color_vc[c] -= rgb_step[c];
	}
	return img;
}

static void DrawCloseImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC3, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, NORMAL_COLOR, RED_COLOR);

	int beg = 4, end = 24, step = 2;
	for (int i = beg; i < end - beg; i++) {
		for (int j = 0; j < step; j++) {
			img.at<cv::Vec3b>(i, i + j) = cv::Vec3b(120, 120, 120);
			img_hover.at<cv::Vec3b>(i, i + j) = cv::Vec3b(120, 120, 120);
			img.at<cv::Vec3b>(i, end - i - j) = cv::Vec3b(120, 120, 120);
			img_hover.at<cv::Vec3b>(i, end - i - j) = cv::Vec3b(120, 120, 120);
		}
	}
	cv::imwrite("./res/close.png", img);
	cv::imwrite("./res/close_hover.png", img_hover);
}

static void DrawMaxImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC3, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, NORMAL_COLOR, RED_COLOR);

	int top = 4, mid = 12, bot = 2, wid = 18, pix = 2;
	for (int i = 3; i < 24; i++) {
		for (int j = 3; j < 21; j++) {
			if (top <= 0 && bot <= 0)continue;
			else if (top <= 0 && mid > 0 && !(abs(j - 3 < pix) || abs(j - 20) < pix))continue;
			img.at<cv::Vec3b>(i, j) = cv::Vec3b(120, 120, 120);
			img_hover.at<cv::Vec3b>(i, j) = cv::Vec3b(120, 120, 120);
		}
		if (top > 0)top--;
		else if(mid > 0) mid--;
		else bot--;
	}
	cv::imwrite("./res/max.png", img);
	cv::imwrite("./res/max_hover.png", img_hover);
}

static void DrawFullImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC3, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, NORMAL_COLOR, RED_COLOR);

	int begin_x = 2, begin_y = 6;
	auto raw_color = NORMAL_COLOR;
	int box_size = 14, offset = 4;
	auto color_vr = cv::Vec3b(raw_color[0], raw_color[1], raw_color[2]);

	for (int i = begin_y; i < begin_y + box_size; i++) {
		bool line = i == begin_y || i == begin_y + box_size - 1;
		for (int j = begin_x; j < begin_x + box_size; j++) {
			if (!line && j != begin_x && j != begin_x + box_size - 1)continue;
			img.at<cv::Vec3b>(i, j) = img_hover.at<cv::Vec3b>(i, j) = cv::Vec3b(120, 120, 120);

			if (i - offset > begin_y && j + offset < begin_x + box_size)continue;
			img.at<cv::Vec3b>(i - offset, j + offset) = cv::Vec3b(120, 120, 120);
			img_hover.at<cv::Vec3b>(i - offset, j + offset) = cv::Vec3b(120, 120, 120);
		}
	}
	cv::imwrite("./res/full.png", img);
	cv::imwrite("./res/full_hover.png", img_hover);
}

static void DrawMinImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC3, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, NORMAL_COLOR, RED_COLOR);

	int width = 18, height = 6;
	int begin_cols = (24 - width) / 2;
	int begin_rows = (24 - height) / 2;
	for (int i = begin_rows; i < begin_rows + height; i++, height--) {
		for (int j = begin_cols; j < begin_cols + width; j++) {
			img.at<cv::Vec3b>(i, j) = cv::Vec3b(120, 120, 120);
			img_hover.at<cv::Vec3b>(i, j) = cv::Vec3b(120, 120, 120);
		}
	}
	cv::imwrite("./res/min.png", img);
	cv::imwrite("./res/min_hover.png", img_hover);
}

static void DrawHelpImg() {
	cv::Mat img = cv::Mat(24, 24, CV_8UC3, NORMAL_COLOR);
	cv::Mat img_hover = GetToolButtonHoverImg(24, 24, NORMAL_COLOR, RED_COLOR);
	cv::Mat img_hover_draw = img_hover.clone();

	cv::circle(img, { 12,12 }, 10, { 120, 120, 120 }, -1, 15);
	cv::circle(img_hover_draw, { 12,12 }, 10, { 120, 120, 120 }, -1, 15);

	cv::putText(img, "?", { 4,20 }, 5, 1.2, { 94, 78, 71}, 2, 15);
	cv::putText(img_hover_draw, "?", { 4,20 }, 5, 1.2, { 94, 78, 71 }, 2, 15);

	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 24; j++) {
			if (img_hover_draw.at<cv::Vec3b>(i, j)[0] == 94)continue;
			if (img_hover_draw.at<cv::Vec3b>(i, j)[1] == 78)continue;
			if (img_hover_draw.at<cv::Vec3b>(i, j)[2] == 71)continue;
			img_hover.at<cv::Vec3b>(i, j) = img_hover_draw.at<cv::Vec3b>(i, j);
		}
	}

	cv::imwrite("./res/help.png", img);
	cv::imwrite("./res/help_hover.png", img_hover);
}

static void DrawAllImg() {
	DrawCloseImg();
	DrawHelpImg();
	DrawFullImg();
	DrawMinImg();
	DrawMaxImg();
}

static void CheckResourceFile() {

}

static void UnPackResourceFile() {

}

#endif // CREATE_RES_H