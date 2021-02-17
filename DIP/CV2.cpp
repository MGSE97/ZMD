#include "CV2.h"

// trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\n' && ch != '\r';
        }));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\n' && ch != '\r';
        }).base(), s.end());
}

// trim from both ends (in place)
static inline std::string trim(std::string& s) {
    ltrim(s);
    rtrim(s);
    return s;
}

int filterChannel(char ch)
{
    int c;
    switch (ch)
    {
    case 'R':
        c = 0;
        break;
    case 'G':
        c = 1;
        break;
    case 'B':
        c = 2;
        break;
    default:
        c = 0;
    }
    return c;
}

cv::Mat BuildFilterMask(const char* filter)
{
    // Parse filter
    std::vector<std::string> lines;
    int size = 0;
    std::stringstream ss(filter);
    std::string to;
    while (std::getline(ss, to, '\n')) {
        trim(to);
        if (to.size() > 0)
        {
            lines.push_back(to);
            if (to.size() > size)
                size = to.size();
        }
    }

    cv::Mat mask = cv::Mat::zeros(cv::Size(size, lines.size()), CV_8UC1);

    for (int y = 0; y < lines.size(); y++)
    {
        auto line = lines[y];
        auto lsize = line.size();
        for (int x = 0; x < size; x++)
        {
            auto channel = lsize > x ? line[x] : '\0';
            mask.at<unsigned char>(y, x) = filterChannel(channel);
        }
    }

    return mask;
}

unsigned char GetSafeValue(cv::Mat* image, int x, int y)
{
    if (x < 0 || x >= image->cols || y < 0 || y >= image->rows)
        return 0;

    return image->at<unsigned char>(y, x);
}

unsigned char GetSafeValue(cv::Mat* image, int x, int y, int cols, int rows)
{
    if (x < 0 || y < 0)
        return 0;

    return image->at<unsigned char>(y % rows, x % cols);
}

cv::Mat BayerToRGB(cv::Mat* image, const char* filter, int lookup_size = 3)
{
    cv::Mat result = cv::Mat::zeros(cv::Size(image->cols, image->rows), CV_8UC3);

    cv::Mat mask = BuildFilterMask(filter);
    //cv::namedWindow("Mask", CV_WINDOW_KEEPRATIO);
    //cv::imshow("Mask", mask*100);

    int min = -(lookup_size / 2), max = lookup_size / 2 + 1,
        mc = mask.cols - 1, mr = mask.rows - 1,
        sx, sy;
    for (int x = 0; x < image->cols; x++)
        for (int y = 0; y < image->rows; y++)
        {
            cv::Vec3i color(0, 0, 0);
            cv::Vec3b count(0, 0, 0);
            for (int dx = min; dx < max; dx++)
            {
                sx = x + dx;
                for (int dy = min; dy < max; dy++)
                {
                    sy = y + dy;
                    auto img = GetSafeValue(image, sx, sy);
                    if (img > 0)
                    {
                        auto col = GetSafeValue(&mask, sx, sy, mc, mr);
                        color[col] += img;
                        count[col] += 1;
                    }
                }
            }

            for (int i = 0; i < 3; i++)
                if (count[i] > 0) color[i] = (int)(color[i] / (float)count[i]);

            result.at<cv::Vec3b>(y, x) = cv::Vec3b((unsigned char)color[0], (unsigned char)color[1], (unsigned char)color[2]);
        }


    return result;
}

void CV2()
{
    auto img = cv::imread("data/bayer.bmp", 0);
    cv::imshow("Bayer", img);

    auto filter = R"(
        RGRGR
        GBGBG
        RGRGR
        GBGBG
        RGRGR
    )";
    auto rgb = BayerToRGB(&img, filter);

    cv::cvtColor(rgb, rgb, cv::COLOR_RGB2BGR);
    cv::imshow("RGB", rgb);
}