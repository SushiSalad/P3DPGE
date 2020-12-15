#pragma once

//math constants
#define M_PI			3.14159265359f
#define M_E				2.71828182846f
#define M_TWOTHIRDS		0.66666666666f
#define M_ONETWELFTH	0.08333333333f

//conversions
#define TO_RADIANS (M_PI / 180.f)
#define TO_DEGREES (180.f / M_PI)

//physics constants
#define GRAVITY 9.81f

//typedefs
typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;
typedef signed long		int64;
typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long	uint64;

//static defines
#define static_internal	static
#define local_persist	static
#define global_variable static
