#include "GdiplusUtils.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

Gdiplus::Status MergeToBitmap(HBITMAP _handle, HBITMAP _alpha, int _width, int _height, Gdiplus::Bitmap** _out) {
	(*_out) = new Gdiplus::Bitmap(_width, _height, PixelFormat32bppARGB);

	HDC hDC = GetDC(NULL);
	HDC hColDC = CreateCompatibleDC(hDC);
	HDC hAlphaDC = CreateCompatibleDC(hDC);
	HBITMAP hOld = (HBITMAP)SelectObject(hColDC, _handle);
	HBITMAP hOld2 = (HBITMAP)SelectObject(hAlphaDC, _alpha);

	for(int x = 0; x < _width; x++){
		for(int y = 0; y < _height; y++){
			COLORREF col = GetPixel(hColDC,x,y);
			COLORREF alpha = GetPixel(hAlphaDC,x,y);
			(*_out)->SetPixel(x, y, Gdiplus::Color( GetRValue(alpha), GetRValue(col), GetGValue(col), GetBValue(col)));
		}
	}

	SelectObject(hColDC, hOld);
	SelectObject(hAlphaDC, hOld2);
	DeleteDC(hColDC);
	DeleteDC(hAlphaDC);
	ReleaseDC(NULL, hDC);

	return Gdiplus::Ok;
}

Gdiplus::Status SaveToFile(Gdiplus::Bitmap* _bmp, LPTSTR _filename) {
	CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    Gdiplus::Status s = _bmp->Save(_filename, &pngClsid, NULL);
	return s;
}