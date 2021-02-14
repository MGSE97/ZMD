#include "CV1.h";

auto YUV = cv::Matx33f(
    /*0.2126, -0.09991, 0.615,
    0.7152, -0.33609, 0.55861,
    0.0722, 0.436, -0.05639*/
    0.299, -0.147, 0.615,
    0.587, -0.289, -0.515,
    0.114, 0.436, -0.1
);

auto RGB = cv::Matx33f(
    /*1.0, 1.0, 1.0,
    0.0, -0.21482, 2.12798,
    1.28033, -0.38059, 0.0*/
    1, 1, 1,
    0, -0.397, 2.034,
    1.137, -0.58, 0
);

cv::Vec3f GetSafeValue(cv::Mat* image, int x, int y)
{
    if (x < 0 || x >= image->cols || y < 0 || y >= image->rows)
        return { 0,0,0 };

    switch (image->type())
    {
    case CV_32FC3:
        return image->at<cv::Vec3f>(y, x);
    case CV_8UC3:
        auto v3b = image->at<cv::Vec3b>(y, x).val;
        return cv::Vec3f((float)v3b[0] / 255.f, (float)v3b[1] / 255.f, (float)v3b[2] / 255.f);
    }
}

void MultiplyByMatrix(cv::Mat* image, cv::Mat* result, cv::Matx33f matrix)
{
    for (int y = 0; y < image->rows; y++)
        for (int x = 0; x < image->cols; x++)
        {
            auto val = GetSafeValue(image, x, y);
            result->at<cv::Vec3f>(y, x) = matrix * val;
        }
}

cv::Mat ToYUV(cv::Mat* image)
{
    cv::Mat result(image->rows, image->cols, CV_32FC3);
    MultiplyByMatrix(image, &result, YUV);
    return result;
}

cv::Mat ToRGB(cv::Mat* image)
{
    cv::Mat result(image->rows, image->cols, CV_32FC3);
    MultiplyByMatrix(image, &result, RGB);
    return result;
}

void CV1()
{
    //auto img = cv::imread("data/lena.png");
    auto img = cv::imread("data/flag.png");
    //auto img = cv::imread("data/valve.png");
    //auto img = cv::imread("data/vsb.jpg");

    cv::imshow("RGB", img);
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

    img = ToYUV(&img);
    cv::imshow("YUV Converted", img);

    img = ToRGB(&img);
    cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
    cv::imshow("RGB Converted", img);
}