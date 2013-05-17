#pragma once
#include <windows.h>
#include <GdiPlus.h>

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
Gdiplus::Status MergeToBitmap(HBITMAP _handle, HBITMAP _alpha, int _width, int _height, Gdiplus::Bitmap** _out);
Gdiplus::Status SaveToFile(Gdiplus::Bitmap* _bmp, LPTSTR _filename);