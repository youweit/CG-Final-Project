#define _CRT_SECURE_NO_WARNINGS 0

#include "header/Angel.h"
//#include <EGL/egl.h>
//#include <EGL/eglext.h>
#include "LibPng/png.h"
#include "SOIL/SOIL.h"

//--------------------------------------------------------------------------------------------
// png_load_SOIL uses SOIL to load png files
//
GLuint png_load_SOIL(const char * file_name, int * width, int * height, bool bMipMap)
{
    GLuint texture;
	GLint iwidth, iheight, ichannel, iformat, row_bytes;
	GLubyte *texArray, *imageData;
	texArray = SOIL_load_image(file_name, &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
	row_bytes = iwidth*ichannel;
	imageData = (GLubyte *)malloc(row_bytes*iheight*sizeof(GLubyte));

	// 對 texArray 執行垂直鏡射，放到 imageData 中
    for (int i = 0; i < iheight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
    }
	switch(ichannel) 
	{
		case 3:
			iformat = GL_RGB;
			break;
		case 4:
			iformat = GL_RGBA;
			break;
		default:
			fprintf(stderr, "%s: Unknown libpng with %d channel.\n", file_name, ichannel);
			return 0;
			break;
	}

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	if( bMipMap ) { // 開啟 MipMap 功能
		glTexImage2D(GL_TEXTURE_2D, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else { // 沒有使用 MipMap 功能
		glTexImage2D(GL_TEXTURE_2D, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
    glBindTexture(GL_TEXTURE_2D, 0);

    // clean up
    free(texArray);
	free(imageData);
    return texture;
}
//--------------------------------------------------------------------------------------------

GLuint CubeMap_load_SOIL()
{
    GLuint texture;
	GLint iwidth, iheight, ichannel, iformat, row_bytes;
	GLubyte *texArray, *imageData;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// 預設讀取 Sunny_NX Sunny_PX Sunny_NY Sunny_PY Sunny_NZ Sunny_PZ 六張影像, 都是 png
	// 每一張影像大小都相同, 所以 imageData 取得一次就行
	texArray = SOIL_load_image("texture/Sunny_PX.png", &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
	row_bytes = iwidth*ichannel;
	imageData = (GLubyte *)malloc(row_bytes*iheight*sizeof(GLubyte));
	// 對 texArray 執行垂直鏡射，放到 imageData 中
    for (int i = 0; i < iheight; i++) memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
	iformat = GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
    free(texArray); // 釋放
	
	texArray = SOIL_load_image("texture/Sunny_NX.png", &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
    for (int i = 0; i < iheight; i++) memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
	free(texArray); // 釋放

	// 因為配合圖檔的垂直鏡射 所以 PY 與 NY 必須對調
	texArray = SOIL_load_image("texture/Sunny_NY.png", &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
    for (int i = 0; i < iheight; i++) memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
	free(texArray); // 釋放

	texArray = SOIL_load_image("texture/Sunny_PY.png", &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
    for (int i = 0; i < iheight; i++) memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
	free(texArray); // 釋放

	texArray = SOIL_load_image("texture/Sunny_PZ.png", &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
    for (int i = 0; i < iheight; i++) memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
	free(texArray); // 釋放

	texArray = SOIL_load_image("texture/Sunny_NZ.png", &iwidth, &iheight, &ichannel, SOIL_LOAD_AUTO);
    for (int i = 0; i < iheight; i++) memcpy(imageData+(row_bytes*i), texArray+(iheight-i-1)*row_bytes, row_bytes);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, iformat, iwidth, iheight, 0, iformat, GL_UNSIGNED_BYTE, imageData);
	free(texArray); // 釋放

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	free(imageData);
    return texture;
}

//--------------------------------------------------------------------------------------------
// png_load_LIBPNG uses libpng to load png files
//
GLuint png_load_LIBPNG(const char * file_name, int * width, int * height, bool bMipMap)
{
    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type, interlace_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        &interlace_type, NULL, NULL);

    if (width){ *width = temp_width; }
    if (height){ *height = temp_height; }

    //printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

    if (bit_depth != 8)
    {
        fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
        return 0;
    }

    GLint format;
    switch(color_type)
    {
    case PNG_COLOR_TYPE_RGB:
        format = GL_RGB;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        format = GL_RGBA;
        break;
    default:
        fprintf(stderr, "%s: Unknown libpng color type %d.\n", file_name, color_type);
        return 0;
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (unsigned int i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	if( bMipMap ) { // 開啟 MipMap 功能
		glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		
	}
	else { // 沒有使用 MipMap 功能
		glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
    glBindTexture(GL_TEXTURE_2D, 0);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}
//
//--------------------------------------------------------------------------------------------