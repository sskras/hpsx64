/*
	Copyright (C) 2012-2030

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


// need a define to toggle sse2 on and off for now
// on 64-bit systems, sse2 is supposed to come as standard
//#define _ENABLE_SSE2
#define _ENABLE_SSE41
//#define _ENABLE_AVX2

#include "WinApiHandler.h"

#include "PS2_Gpu.h"
#include <math.h>
#include "PS2_Timer.h"
#include "Reciprocal.h"

// need to restart dma#2 when path3 is un-masked
#include "PS2_Dma.h"


#include "VU.h"



//#ifdef _ENABLE_SSE41
//#include <emmintrin.h>
//#endif



using namespace Playstation2;
//using namespace x64Asm::Utilities;
using namespace Math::Reciprocal;



//#define USE_DIVIDE_GCC
//#define USE_MULTIPLY_CUSTOM


#define ENABLE_THREAD_LOCK
#define ENABLE_THREAD_LOCK_DATA


#define USE_NEW_INPUTFORMAT
#define ENABLE_PACKET_PRIORITY



// just spin or spin and then wait ??
#define ENABLE_WAIT_DURING_SPIN_FINISH
#define ENABLE_WAIT_DURING_SPIN_THREAD
#define ENABLE_WAIT_DURING_SPIN_DRAWSCREEN



// test mipmap
#define ENABLE_MIPMAP


// can either draw screen at vblank start or vblank end?
#define ENABLE_SCREEN_DRAW_VBLANK_START
//#define ENABLE_SCREEN_DRAW_VBLANK_END






#ifdef _ENABLE_SSE41

//#define ENABLE_AVX128


#define _ENABLE_SSE41_RECTANGLE
#define _ENABLE_SSE41_SPRITE
#define _ENABLE_SSE41_TRIANGLE_MONO
#define _ENABLE_SSE41_TRIANGLE_GRADIENT
#define _ENABLE_SSE41_TRIANGLE_TEXTURE
#define _ENABLE_SSE41_TRIANGLE_TEXTURE_GRADIENT


#endif






#ifdef _ENABLE_AVX2

#define _ENABLE_AVX2_RECTANGLE
#define _ENABLE_AVX2_SPRITE
#define _ENABLE_AVX2_TRIANGLE_MONO
#define _ENABLE_AVX2_TRIANGLE_GRADIENT
#define _ENABLE_AVX2_TRIANGLE_TEXTURE
//#define _ENABLE_AVX2_TRIANGLE_TEXTURE_GRADIENT

#endif


//#define USE_TEMPLATES_RECTANGLE
//#define USE_TEMPLATES_RECTANGLE8
//#define USE_TEMPLATES_RECTANGLE16
//#define USE_TEMPLATES_SPRITE
//#define USE_TEMPLATES_SPRITE8
//#define USE_TEMPLATES_SPRITE16
//#define USE_TEMPLATES_TRIANGLE_MONO
//#define USE_TEMPLATES_RECTANGLE_MONO
//#define USE_TEMPLATES_TRIANGLE_TEXTURE
//#define USE_TEMPLATES_RECTANGLE_TEXTURE
//#define USE_TEMPLATES_TRIANGLE_GRADIENT
//#define USE_TEMPLATES_RECTANGLE_GRADIENT
//#define USE_TEMPLATES_TRIANGLE_TEXTUREGRADIENT
//#define USE_TEMPLATES_RECTANGLE_TEXTUREGRADIENT


//#define COUT_INDEX_RANGE
//#define DISABLE_FIELD
//#define DISABLE_NFIELD
//#define DISABLE_VSINT
//#define DISABLE_HSINT
//#define DISABLE_VSYNC_INT
//#define DISABLE_VSYNCE_INT


//#define VERBOSE_INVALID_REG_WRITE
//#define VERBOSE_FINISH_WRITE
//#define VERBOSE_LABEL_WRITE
//#define VERBOSE_HWREG_WRITE


//#define VERBOSE_READBLOCK

//#define VERBOSE_PATH1_NOTDATA128


//#define VERBOSE_ALERTS_SBW
#define VERBOSE_ALERTS_DBW

#define VERBOSE_PATH3MASK

//#define VERBOSE_TRANSFER_IN
//#define VERBOSE_TRANSFER_OUT
//#define VERBOSE_TRANSFER_LOCAL
//#define VERBOSE_TRANSFER_LOCAL_LIMIT
#define VERBOSE_MSKPATH3

//#define VERBOSE_LOCAL_TRANSFER
//#define VERBOSE_SPRITE_ERROR
//#define VERBOSE_RGBONLY


//#define DISABLE_MONO_TRIANGLE_ALPHA
//#define DISABLE_GRADIENT_TRIANGLE_ALPHA
//#define DISABLE_RECTANGLE_ALPHA
//#define DISABLE_SPRITE_ALPHA


#define ENABLE_ALPHA_POINT
#define ENABLE_ALPHA_LINE_MONO
#define ENABLE_ALPHA_LINE_GRADIENT
#define ENABLE_ALPHA_RECTANGLE
#define ENABLE_ALPHA_SPRITE
#define ENABLE_ALPHA_TRIANGLE_MONO
#define ENABLE_ALPHA_TRIANGLE_GRADIENT
#define ENABLE_ALPHA_TRIANGLE_TEXTURE

#define ENABLE_DEPTH_TEST
#define ENABLE_DEST_ALPHA_TEST
#define ENABLE_SRC_ALPHA_TEST


#define ENABLE_3D_TMAPPING_FTRI
#define ENABLE_3D_TMAPPING_GTRI



// disables allowing path1 to wrap around vu ram or exit it
// must also be using the new args for this to work
#define DISABLE_PATH1_WRAP
#define ENABLE_PATH1_BUFFER




//#define ENABLE_PATH_PROGRESS_ALERT




//#define ENABLE_TRANSPARENT24


//#define ENABLE_INVERT_ZVALUE

// shifts xyz values when converting from 24-bit to 32-bit if enabled
//#define ENABLE_XYZ_SHIFT



//#define VERBOSE_TRIANGLE_ST

//#define VERBOSE_PRIMITIVE_RESERVED


#define USE_NEW_SYNC_PS2TIMERS



#define ENABLE_DATA_STRUCTURE



// enable debugging

#ifdef _DEBUG_VERSION_

#define INLINE_DEBUG_ENABLE

//#define INLINE_DEBUG_SPLIT


//#define DEBUG_TEST


//#define INLINE_DEBUG_CLUT


//#define INLINE_DEBUG_P1BUFFER
//#define INLINE_DEBUG_PROCESSBLOCKS

/*
//#define INLINE_DEBUG_RASTER_VBLANK_START
//#define INLINE_DEBUG_RASTER_VBLANK_END

//#define INLINE_DEBUG_RASTER_SCANLINE
//#define INLINE_DEBUG_HBLANK_INT
#define INLINE_DEBUG_PATH1_WRITE
#define INLINE_DEBUG_PATH2_WRITE



#define INLINE_DEBUG_READ
#define INLINE_DEBUG_WRITE



#define INLINE_DEBUG_DMA_WRITE

//#define INLINE_DEBUG_DMA_WRITE_READY



#define INLINE_DEBUG_PRIMITIVE


#define INLINE_DEBUG_PRIMITIVE_RESERVED



#define INLINE_DEBUG_FIFO
//#define INLINE_DEBUG_FIFO_MT


//#define INLINE_DEBUG_PATH1_WRITE2



//#define INLINE_DEBUG_XFER

//#define INLINE_DEBUG_TRXDIR



//#define INLINE_DEBUG_SPRITE
//#define INLINE_DEBUG_SPRITE_TEST


#define INLINE_DEBUG_INVALID



//#define INLINE_DEBUG_INVALID_SBW
//#define INLINE_DEBUG_INVALID_DBW


//#define INLINE_DEBUG_DRAW_SCREEN





//#define DEBUG_PAGE_CACHE


#define INLINE_DEBUG_PRIMITIVE_TEST

//#define INLINE_DEBUG_DRAWKICK



#define INLINE_DEBUG_TRANSFER_IN
#define INLINE_DEBUG_TRANSFER_IN_2
#define INLINE_DEBUG_TRANSFER_OUT
//#define INLINE_DEBUG_TRANSFER_OUT_2
#define INLINE_DEBUG_TRANSFER_LOCAL
//#define INLINE_DEBUG_TRANSFER_LOCAL_2



//#define INLINE_DEBUG_RECTANGLE_PIXEL



#define INLINE_DEBUG_EXECUTE

//#define INLINE_DEBUG_DRAWSTART
//#define INLINE_DEBUG_EVENT
//#define INLINE_DEBUG_VARS
#define INLINE_DEBUG_EXECUTE_NAME


//#define INLINE_DEBUG_DISPLAYAREA
//#define INLINE_DEBUG_DISPLAYMODE
//#define INLINE_DEBUG_DISPLAYENABLE
//#define INLINE_DEBUG_DISPLAYOFFSET
//#define INLINE_DEBUG_DISPLAYRANGE
//#define INLINE_DEBUG_MASK
//#define INLINE_DEBUG_WINDOW

//#define INLINE_DEBUG_TRIANGLE_MONO_PIXELSDRAWN
//#define INLINE_DEBUG_TRIANGLE_TEXTURE
//#define INLINE_DEBUG_TEXTURE_RECTANGLE
//#define INLINE_DEBUG_PARTIAL_TRIANGLE

//#define INLINE_DEBUG_RUN_MONO
//#define INLINE_DEBUG_RUN_SHADED
//#define INLINE_DEBUG_RUN_TEXTURE
//#define INLINE_DEBUG_RUN_SPRITE
//#define INLINE_DEBUG_RUN_TRANSFER

//#define INLINE_DEBUG_RASTER


//#define INLINE_DEBUG
//#define INLINE_DEBUG_DMA_READ
//#define INLINE_DEBUG_TRIANGLE_MONO
//#define INLINE_DEBUG_TRIANGLE_GRADIENT
//#define INLINE_DEBUG_TRIANGLE_GRADIENT_TEST
//#define INLINE_DEBUG_TRIANGLE_TEXTURE
//#define INLINE_DEBUG_TRIANGLE_TEXTURE_GRADIENT
//#define INLINE_DEBUG_TRIANGLE_MONO_TEST

//#define INLINE_DEBUG_SPRITE_PIXEL
*/


#define INLINE_DEBUG_UNKNOWN_PIXEL


#endif

u32 GPU::ulThreadedGPU;
u32 GPU::ulNumberOfThreads;
u32 GPU::ulNumberOfThreads_Created = 0;
Api::Thread* GPU::GPUThreads [ GPU::c_iMaxThreads ];

volatile u32 GPU::lVertexFlag;

u64 volatile GPU::ullElapsedTime, GPU::ullStartTime, GPU::ullEndTime;
u64 GPU::ullTotalElapsedTime;

u64 GPU::ulTBufferIndex;
u64 GPU::ulP1BufferIndex;


GPU::TEST_t *GPU::pTest;

u32* GPU::_DebugPC;
u64* GPU::_DebugCycleCount;
u32* GPU::_NextEventIdx;

//u32* GPU::_Intc_Master;
u32* GPU::_Intc_Stat;
u32* GPU::_Intc_Mask;
u32* GPU::_R5900_Status_12;
u32* GPU::_R5900_Cause_13;
u64* GPU::_ProcStatus;

//GPU::t_InterruptCPU GPU::InterruptCPU;

//u32 GPU::RAM32_Shadow [ GPU::c_iRAM_Size >> 2 ] __attribute__ ((aligned (32)));


GPU* GPU::_GPU;


u64* GPU::_NextSystemEvent;


// needs to be removed sometime - no longer needed
u32* GPU::DebugCpuPC;


WindowClass::Window *GPU::DisplayOutput_Window;
WindowClass::Window *GPU::FrameBuffer_DebugWindow;

u32 GPU::MainProgramWindow_Width;
u32 GPU::MainProgramWindow_Height;


bool GPU::DebugWindow_Enabled;
//WindowClass::Window *GPU::DebugWindow;

// dimension 1 is twx/twy, dimension #2 is window tww/twh, dimension #3 is value
//u8 GPU::Modulo_LUT [ 32 ] [ 32 ] [ 256 ];

void *GPU::PtrEnd;
void *GPU::PtrEndC;



u64 GPU::ullHwInputBuffer_Index;
u32 GPU::ulHwInputBuffer_Count;
u64 GPU::pHwInputBuffer64 [ ( 1 << GPU::c_ulHwInputBuffer_Shift ) * GPU::c_ulHwInputBuffer_Size ] __attribute__ ((aligned (32)));
u32 *GPU::p_uHwInputData32;
u32 *GPU::p_uHwClutData32;
u32 *GPU::p_uHwCbpxData32;
u32 *GPU::p_uHwOutputData32;

u64 GPU::ulHwInputBuffer_WriteIndex;
u64 GPU::ulHwInputBuffer_TargetIndex;
u64 GPU::ulHwInputBuffer_ReadIndex;


volatile u64 GPU::ullPixelInBuffer_WriteIndex;
volatile u64 GPU::ullPixelInBuffer_TargetIndex;
volatile u64 GPU::ullPixelInBuffer_ReadIndex;
u32 GPU::ulPixelInBuffer32 [ GPU::c_ullPixelInBuffer_Size ] __attribute__ ((aligned (32)));
u32 *GPU::p_ulHwPixelInBuffer32;


//u32 GPU::ulNumberOfThreads;
//static Api::Thread* GPU::GPUThreads [ GPU::c_iMaxThreads ];

u64 GPU::ullInputBuffer_Index;
u32 GPU::ulInputBuffer_Count;


u64 GPU::ulInputBuffer_WriteIndex;
u64 GPU::ulInputBuffer_TargetIndex;
u64 GPU::ulInputBuffer_TargetIndex2;
u64 GPU::ulInputBuffer_ReadIndex;

volatile u64 GPU::ullP1Buf_WriteIndex;
volatile u64 GPU::ullP1Buf_TargetIndex;
volatile u64 GPU::ullP1Buf_TargetIndex2;
volatile u64 GPU::ullP1Buf_ReadIndex;

volatile u64 GPU::ullPath1Buffer [ GPU::c_iPath1BufSize << 1 ] __attribute__ ((aligned (32)));

volatile u64 GPU::display_inputbuffer [ 16 ] __attribute__ ((aligned (32)));

volatile u64 GPU::ullFrameDrawPending;

volatile u64 GPU::inputdata [ ( 1 << GPU::c_ulInputBuffer_Shift ) * GPU::c_ulInputBuffer_Size ] __attribute__ ((aligned (32)));

u32 *GPU::buf32;
u32 *GPU::zbuf32;

u32 *GPU::zbuf_NULL;


Debug::Log GPU::debug;



GPU::GPUDrawingVars GPU::GpDrawVars [ 2 ];


HANDLE GPU::ghEvent_PS2GPU_Update;
HANDLE GPU::ghEvent_PS2GPU_Frame;
HANDLE GPU::ghEvent_PS2GPU_Finish;



/*
static s32 GPU::g_iClamp [ 2 ];
static s32 GPU::g_iClamp_ModeX [ 2 ], GPU::g_iClamp_ModeY [ 2 ];
static s32 GPU::g_iClamp_MinU [ 2 ], GPU::g_iClamp_MaxU [ 2 ];
static s32 GPU::g_iClamp_MinV [ 2 ], GPU::g_iClamp_MaxV [ 2 ];



static s32 GPU::g_iTexY_And [ 2 ], GPU::g_iTexY_Or [ 2 ], GPU::g_iTexY_Min [ 2 ], GPU::g_iTexY_Max [ 2 ];
static s32 GPU::g_iTexX_And [ 2 ], GPU::g_iTexX_Or [ 2 ], GPU::g_iTexX_Min [ 2 ], GPU::g_iTexX_Max [ 2 ];


u32 GPU::g_iTexBufWidth [ 2 ], GPU::g_iTexWidth [ 2 ], GPU::g_iTexHeight [ 2 ];
u32 GPU::g_iTexWidth_Mask [ 2 ], GPU::g_iTexHeight_Mask [ 2 ];


u32 GPU::g_iZBUF_ZBP [ 2 ], GPU::g_iZBUF_PSM [ 2 ], GPU::g_iZBUF_ZMSK [ 2 ];
static u32* GPU::g_pZBUF [ 2 ];

u32 GPU::g_iAlpha_Fix [ 2 ];
u32 GPU::g_iA [ 2 ], GPU::g_iB [ 2 ], GPU::g_iC [ 2 ], GPU::g_iD [ 2 ];

static u64 GPU::g_iTEST_Value [ 2 ];
u32 GPU::g_iTEST_ATE [ 2 ], GPU::g_iTEST_ATST [ 2 ], GPU::g_iTEST_AREF [ 2 ], GPU::g_iTEST_AFAIL [ 2 ], GPU::g_iTEST_ZTE [ 2 ], GPU::g_iTEST_ZTST [ 2 ];
u32 GPU::g_iTEST_DATE [ 2 ], GPU::g_iTEST_DATM [ 2 ];
static AlphaTest GPU::g_iTEST_at [ 2 ];
static ZTest GPU::g_pTEST_zt [ 2 ];
*/


__m128i GPU::vCOLCLAMP;
__m128i GPU::vDATE;
__m128i GPU::vDATM;
__m128i GPU::vZTST_GREATER, GPU::vZTST_EQUAL, GPU::vZTST_LESS;
__m128i GPU::vATST_MASK, GPU::vATST_OFFSET, GPU::vATST_AREF;
__m128i GPU::vAFAIL_PIXEL, GPU::vAFAIL_ZPIXEL;
__m128i GPU::vAEM;


u32 GPU::PixelBuffer [ c_iScreen_MaxWidth * c_iScreen_MaxHeight ] __attribute__ ((aligned (32)));


//u32 GPU::LUT_OFFSET [ 32 * 128 * 128 ];
u32 GPU::LUT_XOFFSET [ 32 * 4096 ];
u32 GPU::LUT_YOFFSET [ 32 * 128 ];
u32 GPU::LUT_XYOFFSET [ 32 * 128 * 128 ];

u32 GPU::LUT_CvtAddrPix32 [ 64 * 32 ];
u32 GPU::LUT_CvtAddrPix16 [ 64 * 64 ];
u32 GPU::LUT_CvtAddrPix16s [ 64 * 64 ];
u32 GPU::LUT_CvtAddrPix8 [ 128 * 64 ];
u32 GPU::LUT_CvtAddrPix4 [ 128 * 128 ];

u32 GPU::LUT_CvtAddrZBuf32 [ 64 * 32 ];
u32 GPU::LUT_CvtAddrZBuf16 [ 64 * 64 ];
u32 GPU::LUT_CvtAddrZBuf16s [ 64 * 64 ];

u32 GPU::ulLUT_OffsetPix32x [ 4096 ] __attribute__ ((aligned (32)));
u32 GPU::ulLUT_OffsetPix16x [ 4096 ] __attribute__ ((aligned (32)));
u32 GPU::ulLUT_OffsetPix16Sx [ 4096 ] __attribute__ ((aligned (32)));
u32 GPU::ulLUT_OffsetPix8x [ 4096 ] __attribute__ ((aligned (32)));
u32 GPU::ulLUT_OffsetPix4x [ 4096 ] __attribute__ ((aligned (32)));

u32 GPU::ulLUT_OffsetZ32x [ 4096 ] __attribute__ ((aligned (32)));
u32 GPU::ulLUT_OffsetZ16x [ 4096 ] __attribute__ ((aligned (32)));
u32 GPU::ulLUT_OffsetZ16Sx [ 4096 ] __attribute__ ((aligned (32)));


const u32 GPU::HBlank_X_LUT [ 8 ] = { 256, 368, 320, 0, 512, 0, 640, 0 };
const u32 GPU::VBlank_Y_LUT [ 2 ] = { 480, 576 };
const u32 GPU::Raster_XMax_LUT [ 2 ] [ 8 ] = { { 341, 487, 426, 0, 682, 0, 853, 0 }, { 340, 486, 426, 0, 681, 0, 851, 0 } };
const u32 GPU::Raster_YMax_LUT [ 2 ] = { 525, 625 };


u64 GPU::CyclesPerPixel_INC_Lookup [ 2 ] [ 8 ];
double GPU::CyclesPerPixel_Lookup [ 2 ] [ 8 ];
double GPU::PixelsPerCycle_Lookup [ 2 ] [ 8 ];


const char* GPU::GIFRegNames [ 11 ] = { "GIF_CTRL", "GIF_MODE", "GIF_STAT", "Reserved", "GIF_TAG0", "GIF_TAG1", "GIF_TAG2", "GIF_TAG3", "GIF_CNT", "GIF_P3CNT", "GIF_P3TAG" };
const char* GPU::GPUReg0Names [ 15 ] = { "GPU_PMODE", "GPU_SMODE1", "GPU_SMODE2", "GPU_SRFSH", "GPU_SYNCH1", "GPU_SYNCH2", "GPU_SYNCV", "GPU_DISPFB1",
												"GPU_DISPLAY1", "GPU_DISPFB2", "GPU_DISPLAY2", "GPU_EXTBUF", "GPU_EXTDATA", "GPU_EXTWRITE", "GPU_BGCOLOR" };
const char* GPU::GPUReg1Names [ 9 ] = { "GPU_CSR", "GPU_IMR", "Reserved", "Reserved", "GPU_BUSDIR", "Reserved", "Reserved", "Reserved", "GPU_SIGLBLID" };

const char* GPU::GPURegsGp_Names [ 0x63 ] = { "PRIM", "RGBAQ", "ST", "UV", "XYZF2", "XYZ2", "TEX0_1", "TEX0_2",	// 0x00-0x07
													"CLAMP_1", "CLAMP_2", "FOG", "", "XYZF3", "XYZ3", "", "",				// 0x08-0x0f
													"", "", "", "", "TEX1_1", "TEX1_2", "TEX2_1", "TEX2_2",					// 0x10-0x17
													"XYOFFSET_1", "XYOFFSET_2", "PRMODECONT", "PRMODE", "TEXCLUT", "", "", "",	// 0x18-0x1f
													"", "", "SCANMSK", "", "", "", "", "",									// 0x20-0x27
													"", "", "", "", "", "", "", "",											// 0x28-0x2f
													"", "", "", "", "MIPTBP1_1", "MIPTBP1_2", "MIPTBP2_1", "MIPTBP2_2",		// 0x30-0x37
													"", "", "", "TEXA", "", "FOGCOL", "", "TEXFLUSH",						// 0x38-0x3f
													"SCISSOR_1", "SCISSOR_2", "ALPHA_1", "ALPHA_2", "DIMX", "DTHE", "COLCLAMP", "TEST_1",	// 0x40-0x47
													"TEST_2", "PABE", "FBA_1", "FBA_2", "FRAME_1", "FRAME_2", "ZBUF_1", "ZBUF_2",	// 0x48-0x4f
													"BITBLTBUF", "TRXPOS", "TRXREG", "TRXDIR", "HWREG", "", "", "",			// 0x50-0x57
													"", "", "", "", "", "", "", "",											// 0x58-0x5f
													"SIGNAL", "FINISH", "LABEL" };											// 0x60-0x62

// 000000: PSMCT32, 000001: PSMCT24, 000010: PSMCT16, 001010: PSMCT16S, 010011: PSMT8, 010100: PSMT4, 011011: PSMT8H,
// 100100: PSMT4HL, 101100: PSMT4HH, 110000: PSMZ32, 110001: PSMZ24, 110010: PSMZ16, 111010: PSMZ16S
const char* GPU::PixelFormat_Names [ 64 ] =  { "PSMCT32", "PSMCT24", "PSMCT16", "UNK", "UNK", "UNK", "UNK", "UNK",	// 0-7
													"UNK", "UNK", "PSMCT16S", "UNK", "UNK", "UNK", "UNK", "UNK",			// 8-15
													"UNK", "UNK", "UNK", "PSMT8", "PSMT4", "UNK", "UNK", "UNK",				// 16-23
													"UNK", "UNK", "UNK", "PSMT8H", "UNK", "UNK", "UNK", "UNK",				// 24-31
													"UNK", "UNK", "UNK", "UNK", "PSMT4HL", "UNK", "UNK", "UNK",				// 32-39
													"UNK", "UNK", "UNK", "UNK", "PSMT4HH", "UNK", "UNK", "UNK",				// 40-47
													"PSMZ32", "PSMZ24", "PSMZ16", "UNK", "UNK", "UNK", "UNK", "UNK",		// 48-55
													"UNK", "UNK", "PSMZ16S", "UNK", "UNK", "UNK", "UNK", "UNK"				// 56-63
													};

const char* GPU::TransferDir_Names [ 4 ] = { "UpperLeft->LowerRight", "LowerLeft->UpperRight", "UpperRight->LowerLeft", "LowerRight->UpperLeft" };



static const unsigned short ucCLUT_Lookup_CSM01 [ 256 ] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
															0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
															0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
															0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
															0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
															0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
															0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
															0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
															0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
															0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
															0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
															0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
															0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
															0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
															0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
															0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };


static const unsigned short ucCLUT_Lookup_CSM01_4bpp [ 256 ] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
																0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
																0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
																0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
																0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x140, 0x141, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147,
																0x108, 0x109, 0x10a, 0x10b, 0x10c, 0x10d, 0x10e, 0x10f, 0x148, 0x149, 0x14a, 0x14b, 0x14c, 0x14d, 0x14e, 0x14f,
																0x180, 0x181, 0x182, 0x183, 0x184, 0x185, 0x186, 0x187, 0x1c0, 0x1c1, 0x1c2, 0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7,
																0x188, 0x189, 0x18a, 0x18b, 0x18c, 0x18d, 0x18e, 0x18f, 0x1c8, 0x1c9, 0x1ca, 0x1cb, 0x1cc, 0x1cd, 0x1ce, 0x1cf,
																0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x247,
																0x208, 0x209, 0x20a, 0x20b, 0x20c, 0x20d, 0x20e, 0x20f, 0x248, 0x249, 0x24a, 0x24b, 0x24c, 0x24d, 0x24e, 0x24f,
																0x280, 0x281, 0x282, 0x283, 0x284, 0x285, 0x286, 0x287, 0x2c0, 0x2c1, 0x2c2, 0x2c3, 0x2c4, 0x2c5, 0x2c6, 0x2c7,
																0x288, 0x289, 0x28a, 0x28b, 0x28c, 0x28d, 0x28e, 0x28f, 0x2c8, 0x2c9, 0x2ca, 0x2cb, 0x2cc, 0x2cd, 0x2ce, 0x2cf,
																0x300, 0x301, 0x302, 0x303, 0x304, 0x305, 0x306, 0x307, 0x340, 0x341, 0x342, 0x343, 0x344, 0x345, 0x346, 0x347,
																0x308, 0x309, 0x30a, 0x30b, 0x30c, 0x30d, 0x30e, 0x30f, 0x348, 0x349, 0x34a, 0x34b, 0x34c, 0x34d, 0x34e, 0x34f,
																0x380, 0x381, 0x382, 0x383, 0x384, 0x385, 0x386, 0x387, 0x3c0, 0x3c1, 0x3c2, 0x3c3, 0x3c4, 0x3c5, 0x3c6, 0x3c7,
																0x388, 0x389, 0x38a, 0x38b, 0x38c, 0x38d, 0x38e, 0x38f, 0x3c8, 0x3c9, 0x3ca, 0x3cb, 0x3cc, 0x3cd, 0x3ce, 0x3cf };

																
																
static const unsigned short ucCLUT_Lookup_CSM01_8bpp [ 256 ] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107,
															0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x108, 0x109, 0x10a, 0x10b, 0x10c, 0x10d, 0x10e, 0x10f,
															0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x300, 0x301, 0x302, 0x303, 0x304, 0x305, 0x306, 0x307,
															0x208, 0x209, 0x20a, 0x20b, 0x20c, 0x20d, 0x20e, 0x20f, 0x308, 0x309, 0x30a, 0x30b, 0x30c, 0x30d, 0x30e, 0x30f,
															0x400, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x500, 0x501, 0x502, 0x503, 0x504, 0x505, 0x506, 0x507,
															0x408, 0x409, 0x40a, 0x40b, 0x40c, 0x40d, 0x40e, 0x40f, 0x508, 0x509, 0x50a, 0x50b, 0x50c, 0x50d, 0x50e, 0x50f,
															0x600, 0x601, 0x602, 0x603, 0x604, 0x605, 0x606, 0x607, 0x700, 0x701, 0x702, 0x703, 0x704, 0x705, 0x706, 0x707,
															0x608, 0x609, 0x60a, 0x60b, 0x60c, 0x60d, 0x60e, 0x60f, 0x708, 0x709, 0x70a, 0x70b, 0x70c, 0x70d, 0x70e, 0x70f,
															0x800, 0x801, 0x802, 0x803, 0x804, 0x805, 0x806, 0x807, 0x900, 0x901, 0x902, 0x903, 0x904, 0x905, 0x906, 0x907,
															0x808, 0x809, 0x80a, 0x80b, 0x80c, 0x80d, 0x80e, 0x80f, 0x908, 0x909, 0x90a, 0x90b, 0x90c, 0x90d, 0x90e, 0x90f,
															0xa00, 0xa01, 0xa02, 0xa03, 0xa04, 0xa05, 0xa06, 0xa07, 0xb00, 0xb01, 0xb02, 0xb03, 0xb04, 0xb05, 0xb06, 0xb07,
															0xa08, 0xa09, 0xa0a, 0xa0b, 0xa0c, 0xa0d, 0xa0e, 0xa0f, 0xb08, 0xb09, 0xb0a, 0xb0b, 0xb0c, 0xb0d, 0xb0e, 0xb0f,
															0xc00, 0xc01, 0xc02, 0xc03, 0xc04, 0xc05, 0xc06, 0xc07, 0xd00, 0xd01, 0xd02, 0xd03, 0xd04, 0xd05, 0xd06, 0xd07,
															0xc08, 0xc09, 0xc0a, 0xc0b, 0xc0c, 0xc0d, 0xc0e, 0xc0f, 0xd08, 0xd09, 0xd0a, 0xd0b, 0xd0c, 0xd0d, 0xd0e, 0xd0f,
															0xe00, 0xe01, 0xe02, 0xe03, 0xe04, 0xe05, 0xe06, 0xe07, 0xf00, 0xf01, 0xf02, 0xf03, 0xf04, 0xf05, 0xf06, 0xf07,
															0xe08, 0xe09, 0xe0a, 0xe0b, 0xe0c, 0xe0d, 0xe0e, 0xe0f, 0xf08, 0xf09, 0xf0a, 0xf0b, 0xf0c, 0xf0d, 0xf0e, 0xf0f };

															
static const unsigned short ucCLUT_Lookup_CSM02 [ 256 ] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
															0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
															0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
															0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
															0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
															0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
															0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
															0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
															0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
															0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
															0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
															0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
															0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
															0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
															0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
															0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };


/*
#define ALLOW_OPENCL_PS2


#define ALLOW_OPENCL_PS2_DRAWSCREEN
#define ALLOW_OPENCL_PS2_SETDRAWVARS
#define ALLOW_OPENCL_PS2_TRANSFERIN
#define ALLOW_OPENCL_PS2_TRANSFERMOVE
#define ENABLE_HWPIXEL_INPUT
//#define ALLOW_OPENCL_PS2_TRANSFEROUT
//#define ALLOW_OPENCL_PS2_LINE
//#define ALLOW_OPENCL_PS2_POINT
#define ALLOW_OPENCL_PS2_WRITECLUT
#define ALLOW_OPENCL_PS2_TRIANGLE_COLOR
#define ALLOW_OPENCL_PS2_TRIANGLE_TEXTURE
#define ALLOW_OPENCL_PS2_RECTANGLE
#define ALLOW_OPENCL_PS2_SPRITE

#define ALLOW_OPENCL_PS2_TEST_CLUT


// choose one here
//#define ENABLE_OPENCL_ACTIVE_RENDER
#define ENABLE_OPENCL_PASSIVE_RENDER
*/


// this needs to be set here and in compute shader
#define PRECOMPUTE_LIST_SIZE	( 1 << 16 )
#define PRECOMPUTE_LIST_MASK	( PRECOMPUTE_LIST_SIZE - 1 )


//#define TEST_GPU_RENDER



GLuint computeProgram;
//GLuint buffers[NUM_BUFS];       //SSBO objects, one for IMG_0, one for IMG_1, and one for commands/response
//static GLchar* computeSource;
GLuint shaderProgram;
//string computeSource;
GLuint ssbo1;
GLuint ssbo;

GLuint ssbo_precompute;
GLuint ssbo_sinputdata;
GLuint ssbo_outputdata;
GLuint ssbo_shadowvram;
GLuint ssbo_pixelbuffer;
GLuint ssbo2;
GLuint ssbo_xlut;
GLuint ssbo_ylut;
//GLuint ssbo_xylut;
GLuint ssbo_clut;
GLuint ssbo_sclut;
GLuint ssbo_cbpx;
GLuint ssbo_scbpx;

GLuint ssbo_sinputpixels;
GLuint ssbo_inputpixels;

int tex_w = 1024, tex_h = 1024;
GLuint tex_output;
GLuint fboId = 0;

//#define GLSL(version,shader) "#version" #version "\n" #shader

static const char* computeSource =
#include "ps2gpu.comp"
;



GPU::GPU ()
{

	cout << "Running GPU constructor...\n";
}

void GPU::Reset ()
{
	// zero object
	memset ( this, 0, sizeof( GPU ) );

	// default value for IMR is 0x7f00
	GPURegs1.IMR.Value = 0x7f00;
	
	// fifo is empty
	GPURegs1.CSR.FIFO = 0x1;
}


void GPU::Init ()
{
	// clear multi-threading buffer
	memset ( (void*) inputdata, 0, sizeof ( u64 ) * ( 1 << c_ulInputBuffer_Shift ) * c_ulInputBuffer_Size );
	
	CyclesPerPixel_INC_Lookup [ 0 ] [ 0 ] = NTSC_CyclesPerPixelINC_256;
	CyclesPerPixel_INC_Lookup [ 0 ] [ 1 ] = NTSC_CyclesPerPixelINC_368;
	CyclesPerPixel_INC_Lookup [ 0 ] [ 2 ] = NTSC_CyclesPerPixelINC_320;
	CyclesPerPixel_INC_Lookup [ 0 ] [ 4 ] = NTSC_CyclesPerPixelINC_512;
	CyclesPerPixel_INC_Lookup [ 0 ] [ 6 ] = NTSC_CyclesPerPixelINC_640;

	CyclesPerPixel_INC_Lookup [ 1 ] [ 0 ] = PAL_CyclesPerPixelINC_256;
	CyclesPerPixel_INC_Lookup [ 1 ] [ 1 ] = PAL_CyclesPerPixelINC_368;
	CyclesPerPixel_INC_Lookup [ 1 ] [ 2 ] = PAL_CyclesPerPixelINC_320;
	CyclesPerPixel_INC_Lookup [ 1 ] [ 4 ] = PAL_CyclesPerPixelINC_512;
	CyclesPerPixel_INC_Lookup [ 1 ] [ 6 ] = PAL_CyclesPerPixelINC_640;
	
	CyclesPerPixel_Lookup [ 0 ] [ 0 ] = NTSC_CyclesPerPixel_256;
	CyclesPerPixel_Lookup [ 0 ] [ 1 ] = NTSC_CyclesPerPixel_368;
	CyclesPerPixel_Lookup [ 0 ] [ 2 ] = NTSC_CyclesPerPixel_320;
	CyclesPerPixel_Lookup [ 0 ] [ 4 ] = NTSC_CyclesPerPixel_512;
	CyclesPerPixel_Lookup [ 0 ] [ 6 ] = NTSC_CyclesPerPixel_640;

	CyclesPerPixel_Lookup [ 1 ] [ 0 ] = PAL_CyclesPerPixel_256;
	CyclesPerPixel_Lookup [ 1 ] [ 1 ] = PAL_CyclesPerPixel_368;
	CyclesPerPixel_Lookup [ 1 ] [ 2 ] = PAL_CyclesPerPixel_320;
	CyclesPerPixel_Lookup [ 1 ] [ 4 ] = PAL_CyclesPerPixel_512;
	CyclesPerPixel_Lookup [ 1 ] [ 6 ] = PAL_CyclesPerPixel_640;
	

	PixelsPerCycle_Lookup [ 0 ] [ 0 ] = NTSC_PixelsPerCycle_256;
	PixelsPerCycle_Lookup [ 0 ] [ 1 ] = NTSC_PixelsPerCycle_368;
	PixelsPerCycle_Lookup [ 0 ] [ 2 ] = NTSC_PixelsPerCycle_320;
	PixelsPerCycle_Lookup [ 0 ] [ 4 ] = NTSC_PixelsPerCycle_512;
	PixelsPerCycle_Lookup [ 0 ] [ 6 ] = NTSC_PixelsPerCycle_640;

	PixelsPerCycle_Lookup [ 1 ] [ 0 ] = PAL_PixelsPerCycle_256;
	PixelsPerCycle_Lookup [ 1 ] [ 1 ] = PAL_PixelsPerCycle_368;
	PixelsPerCycle_Lookup [ 1 ] [ 2 ] = PAL_PixelsPerCycle_320;
	PixelsPerCycle_Lookup [ 1 ] [ 4 ] = PAL_PixelsPerCycle_512;
	PixelsPerCycle_Lookup [ 1 ] [ 6 ] = PAL_PixelsPerCycle_640;

	
	// set pointer to end of draw buffer (need this so variables don't get overwritten)
	PtrEnd = RAM8 + c_iRAM_Size;
	PtrEndC = BUF8 + c_iRAM_Size;

	// init lookup tables for cvt
	InitCvtLUTs ();
	
	
	// new multi-threaded graphics reset code
	ulNumberOfThreads = 0;
	ullInputBuffer_Index = 0;
	ulInputBuffer_Count = 0;

	ulThreadedGPU = 0;
	
	// frame draw not currently pending
	ullFrameDrawPending = 0;
	
	// refresh static variables
	Refresh ();
}



// actually, you need to start objects after everything has been initialized
void GPU::Start ()
{
	cout << "Running GPU::Start...\n";
	
#ifdef INLINE_DEBUG_ENABLE

#ifdef INLINE_DEBUG_SPLIT
	// put debug output into a separate file
	debug.SetSplit ( true );
	debug.SetCombine ( false );
#endif

	debug.Create ( "PS2_GPU_Log.txt" );
#endif

#ifdef INLINE_DEBUG
	debug << "\r\nEntering GPU::Start";
#endif

	cout << "Resetting GPU...\n";

	// resets the GPU object like a reset signal
	Reset ();
	
	// there should be a difference between resetting the GPU object and initializing
	Init ();

	cout << "Preparing PS2 GPU...\n";
	
	///////////////////////////////
	// *** TESTING ***
	//GPU_CTRL_Read.Value = 0x14802000;
	UpdateRaster_VARS ();
	// *** END TESTING ***
	////////////////////////////////
	
	// set as current GPU object
	_GPU = this;
	
	
	// set null z-buffer to empty unused buffer
	zbuf_NULL = BUF32;
	
	// set all transfers to end of packet
	EndOfPacket [ 0 ] = 1;
	EndOfPacket [ 1 ] = 1;
	EndOfPacket [ 2 ] = 1;
	EndOfPacket [ 3 ] = 1;

	// 0 means on same thread, 1 or greater means on separate threads (power of 2 ONLY!!)
	ulNumberOfThreads = 1;
	ulThreadedGPU = 1;
	
	// no threads created yet
	ulNumberOfThreads_Created = 0;
	

	
	
	
	
	// init page cache
	//for ( int i = 0; i < 512; i++ ) ulCachedBufFormat [ i ] = -1;
	
	// init offset luts
	for ( int i = 0; i < 4096; i++ )
	{
		ulLUT_OffsetPix32x [ i ] = CvtAddrPix32 ( i, 0, 0 );
		ulLUT_OffsetPix16x [ i ] = CvtAddrPix16 ( i, 0, 0 );
		ulLUT_OffsetPix16Sx [ i ] = CvtAddrPix16S ( i, 0, 0 );
		ulLUT_OffsetPix8x [ i ] = CvtAddrPix8 ( i, 0, 0 );
		ulLUT_OffsetPix4x [ i ] = CvtAddrPix4 ( i, 0, 0 );
		
		ulLUT_OffsetZ32x [ i ] = CvtAddrZBuf32 ( i, 0x10, 0 );
		ulLUT_OffsetZ16x [ i ] = CvtAddrZBuf16 ( i, 0x20, 0 );
		ulLUT_OffsetZ16Sx [ i ] = CvtAddrZBuf16S ( i, 0x10, 0 );

		// also setup the lookup tables for the gpu shader renderer
		LUT_XOFFSET [ ( 0x0 << 12 ) | i ] = CvtAddrPix32 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0x1 << 12 ) | i ] = CvtAddrPix16 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0x5 << 12 ) | i ] = CvtAddrPix16S ( i, 0, 0 );

		LUT_XOFFSET [ ( 0x9 << 12 ) | i ] = CvtAddrPix8 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0xa << 12 ) | i ] = CvtAddrPix4 ( i, 0, 0 );

		LUT_XOFFSET [ ( 0x0d << 12 ) | i ] = CvtAddrPix32 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0x12 << 12 ) | i ] = CvtAddrPix32 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0x16 << 12 ) | i ] = CvtAddrPix32 ( i, 0, 0 );

		LUT_XOFFSET [ ( 0x18 << 12 ) | i ] = CvtAddrZBuf32 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0x19 << 12 ) | i ] = CvtAddrZBuf16 ( i, 0, 0 );
		LUT_XOFFSET [ ( 0x1d << 12 ) | i ] = CvtAddrZBuf16S ( i, 0, 0 );
	}



#ifdef ALLOW_OPENCL_PS2
	// hooking in opencl //
	
	DisplayOutput_Window->OpenGL_MakeCurrentWindow ();
	

	cout << "\nLoading program...";

	//computeSource = LoadProgram("C:\\Users\\PCUser\\Desktop\\TheProject\\hpsx64\\hps1x64\\src\\gpu\\src\\compute.comp");
    //if (computeSource == NULL)
    //{
    //    return;
    //}

	cout << "\nLoaded:" << (char*)computeSource;

	cout << "\nInit glew...";

	GLenum err = glewInit();

	cout << "\nglewInit()= " << err;

	/*
	if (err != GLEW_OK)
	{
	cout << "Error: " << glewGetErrorString(err);
	//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	//...
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	*/

	cout << "\nCreating compute shader...";

	
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);


	cout << "\nglShaderSource...";

    glShaderSource(computeShader, 1, &computeSource, NULL);

	cout << "\nglCompileShader...";

    glCompileShader(computeShader);

	cout << "\nglCreateProgram...";

    computeProgram = glCreateProgram();

	cout << "\nglAttachShader...";

    glAttachShader(computeProgram, computeShader);

	cout << "\nglLinkProgram...";

    glLinkProgram(computeProgram);

	cout << "\nglGetProgramiv...";

    GLint status;
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &status);

    if (status == GL_TRUE)
    {
        printf("link good\n");
    }
    else
    {
        cout << "link bad\n";
        GLchar log[4096];
        GLsizei len;

        glGetProgramInfoLog(computeProgram, 4096, &len, log);

        cout << log << "\n";
        //return;
    }

	cout << "\nglUseProgram...";

	glUseProgram(computeProgram);

	// input data copy //

	// invalidate all input buffer entries
	memset( pHwInputBuffer64, -1, sizeof(pHwInputBuffer64) );

	glGenBuffers(1, &ssbo_sinputdata);
	glBindBuffer ( GL_SHADER_STORAGE_BUFFER, ssbo_sinputdata );
	glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(pHwInputBuffer64), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
	p_uHwInputData32 = (u32*) glMapBufferRange ( GL_SHADER_STORAGE_BUFFER, 0, sizeof(pHwInputBuffer64), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );


	// input pixel copy data map
	glGenBuffers(1, &ssbo_sinputpixels);
	glBindBuffer ( GL_SHADER_STORAGE_BUFFER, ssbo_sinputpixels );
	glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(ulPixelInBuffer32), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
	p_ulHwPixelInBuffer32 = (u32*) glMapBufferRange ( GL_SHADER_STORAGE_BUFFER, 0, sizeof(ulPixelInBuffer32), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );


	glGenBuffers(1, &ssbo_inputpixels);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, ssbo_inputpixels);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ulPixelInBuffer32), NULL, GL_DYNAMIC_COPY);


#ifdef ALLOW_OPENCL_PS2_TEST_CLUT

	glGenBuffers(1, &ssbo_sclut);
	glBindBuffer ( GL_SHADER_STORAGE_BUFFER, ssbo_sclut );
	glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(InternalCLUT) * 2, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
	p_uHwClutData32 = (u32*) glMapBufferRange ( GL_SHADER_STORAGE_BUFFER, 0, sizeof(InternalCLUT) * 2, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );

	glGenBuffers(1, &ssbo_scbpx);
	glBindBuffer ( GL_SHADER_STORAGE_BUFFER, ssbo_scbpx );
	glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(CBPX), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
	p_uHwCbpxData32 = (u32*) glMapBufferRange ( GL_SHADER_STORAGE_BUFFER, 0, sizeof(CBPX), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );

	// clut
	glGenBuffers(1, &ssbo_clut);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssbo_clut);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(InternalCLUT) * 2, NULL, GL_DYNAMIC_COPY);

	// cbp0/cbp1
	glGenBuffers(1, &ssbo_cbpx);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssbo_cbpx);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(CBPX), NULL, GL_DYNAMIC_COPY);

#endif

	// internal pre-compute buffer //
	//glGenBuffers(1, &ssbo_precompute);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_precompute);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, 128 * PRECOMPUTE_LIST_SIZE * 4, NULL, GL_DYNAMIC_COPY);


	// setup internal command buffer //

	glGenBuffers(1, &ssbo1);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo1);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(pHwInputBuffer64), pHwInputBuffer64, GL_DYNAMIC_COPY);
	//glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(inputdata), NULL, GL_DYNAMIC_STORAGE_BIT );


	// setup VRAM //

	// vram data copy and bind //
	glGenBuffers(1, &ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

	// copy 2x the size of VRAM for now, will update later
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(_GPU->RAM32) * 2, NULL, GL_DYNAMIC_COPY);
	//glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(_GPU->VRAM) * 2, NULL, GL_DYNAMIC_STORAGE_BIT );


	// setup shadow VRAM //
	glGenBuffers(1, &ssbo_shadowvram);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo_shadowvram);

	// copy 2x the size of VRAM for now, will update later
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(_GPU->RAM32) * 2, NULL, GL_DYNAMIC_COPY);
	//glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(_GPU->VRAM) * 2, NULL, GL_DYNAMIC_STORAGE_BIT );

	// setup 32-bit pixel buffer //

	// pixel buffer
	glGenBuffers(1, &ssbo_pixelbuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_pixelbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PixelBuffer), NULL, GL_DYNAMIC_COPY); //sizeof(data) only works for statically sized C/C++ arrays.

	// setup LUTs //

	// x-lut
	glGenBuffers(1, &ssbo_xlut);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssbo_xlut);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LUT_XOFFSET), LUT_XOFFSET, GL_DYNAMIC_COPY);

	// y-lut
	glGenBuffers(1, &ssbo_ylut);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, ssbo_ylut);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LUT_YOFFSET), LUT_YOFFSET, GL_DYNAMIC_COPY);

	// xy-lut
	//glGenBuffers(1, &ssbo_xylut);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, ssbo_xylut);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LUT_XYOFFSET), LUT_XYOFFSET, GL_DYNAMIC_COPY);


	// output data
	glGenBuffers(1, &ssbo_outputdata);
	glBindBuffer ( GL_SHADER_STORAGE_BUFFER, ssbo_outputdata );
	glBufferStorage( GL_SHADER_STORAGE_BUFFER, sizeof(_GPU->RAM32) * 2, NULL, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
	p_uHwOutputData32 = (u32*) glMapBufferRange ( GL_SHADER_STORAGE_BUFFER, 0, sizeof(_GPU->RAM32) * 2, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );


	// internal pre-compute buffer //
	glGenBuffers(1, &ssbo_precompute);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_precompute);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 128 * PRECOMPUTE_LIST_SIZE * 4, NULL, GL_DYNAMIC_COPY);


	// setup output texture and frame buffer //

	// test rendering to texture //
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, tex_w, tex_h, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL);
	glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,tex_w,tex_h);
	glBindImageTexture(5, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_output, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
	//glBlitFramebuffer( 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR );

	//GLenum err;
	while((err = glGetError()) != GL_NO_ERROR)
	{
		// Process/log the error.
		cout << "\n***OPENGL ERROR*** value=" << hex << err;
	}

	GLint size;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &size);
	std::cout << "\nGL_MAX_SHADER_STORAGE_BLOCK_SIZE is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &size);
	std::cout << "\nGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &size);
	std::cout << "\nGL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &size);
	std::cout << "\nGL_MAX_COMPUTE_ATOMIC_COUNTERS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &size);
	std::cout << "\nGL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, &size);
	std::cout << "\nGL_MAX_COMPUTE_WORK_GROUP_COUNT is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, &size);
	std::cout << "\nGL_MAX_COMPUTE_WORK_GROUP_SIZE is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &size);
	std::cout << "\nGL_MAX_COMPUTE_UNIFORM_BLOCKS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &size);
	std::cout << "\nGL_MAX_COMPUTE_UNIFORM_COMPONENTS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &size);
	std::cout << "\nGL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS is " << size << " bytes." << endl;
	glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &size);
	std::cout << "\nGL_MAX_COMPUTE_SHARED_MEMORY_SIZE is " << size << " bytes." << endl;



	DisplayOutput_Window->OpenGL_ReleaseWindow ();


#endif

	// testing opencl
	bEnable_OpenCL = false;

	ulHwInputBuffer_WriteIndex = 0;
	ulHwInputBuffer_ReadIndex = 0;
	ulHwInputBuffer_TargetIndex = 0;

	
	// 0 means on same thread, 1 or greater means on separate threads (power of 2 ONLY!!)
	ulNumberOfThreads = 1;
	ulThreadedGPU = 1;

	
	cout << "done\n";

#ifdef INLINE_DEBUG
	debug << "->Exiting GPU::Start";
#endif

	cout << "Exiting GPU::Start...\n";
}



void GPU::SetDisplayOutputWindow ( u32 width, u32 height, WindowClass::Window* DisplayOutput )
{
	MainProgramWindow_Width = width;
	MainProgramWindow_Height = height;
	DisplayOutput_Window = DisplayOutput;
}



void GPU::Refresh ()
{
	Refresh_ALPHA( ALPHA_1 & 1, GPURegsGp.Regs [ ALPHA_1 ] );
	Refresh_ALPHA( ALPHA_2 & 1, GPURegsGp.Regs [ ALPHA_2 ] );
	
	Refresh_ZBUF( ZBUF_1 & 1, GPURegsGp.Regs [ ZBUF_1 ] );
	Refresh_ZBUF( ZBUF_2 & 1, GPURegsGp.Regs [ ZBUF_2 ] );
	
	Refresh_TEST( TEST_1 & 1, GPURegsGp.Regs [ TEST_1 ] );
	Refresh_TEST( TEST_2 & 1, GPURegsGp.Regs [ TEST_2 ] );
	
	Refresh_CLAMP( CLAMP_1 & 1, GPURegsGp.Regs [ CLAMP_1 ] );
	Refresh_CLAMP( CLAMP_2 & 1, GPURegsGp.Regs [ CLAMP_2 ] );
	
	Refresh_TEX0( TEX0_1 & 1, GPURegsGp.Regs [ TEX0_1 ] );
	Refresh_TEX0( TEX0_2 & 1, GPURegsGp.Regs [ TEX0_2 ] );
	
	Refresh_FRAME( FRAME_1 & 1, GPURegsGp.Regs [ FRAME_1 ] );
	Refresh_FRAME( FRAME_2 & 1, GPURegsGp.Regs [ FRAME_2 ] );
	
	Refresh_FBA( FBA_1 & 1, GPURegsGp.Regs [ FBA_1 ] );
	Refresh_FBA( FBA_2 & 1, GPURegsGp.Regs [ FBA_2 ] );
	
	Refresh_COLCLAMP( 0, GPURegsGp.Regs [ COLCLAMP ] );
	Refresh_PABE( 0, GPURegsGp.Regs [ PABE ] );
	Refresh_DTHE( 0, GPURegsGp.Regs [ DTHE ] );
	Refresh_DIMX( 0, GPURegsGp.Regs [ DIMX ] );
	Refresh_FOGCOL( 0, GPURegsGp.Regs [ FOGCOL ] );
	Refresh_TEXCLUT( 0, GPURegsGp.Regs [ TEXCLUT ] );
	Refresh_TEXA( 0, GPURegsGp.Regs [ TEXA ] );
}



void GPU::Run ()
{
	// want to run the events in the right order, so need to check for exact match with current cycle count
	//if ( NextEvent_Cycle != *_DebugCycleCount ) return;

	// must clear the previous event first?
	
	// must be start of a scanline //	
	
#ifdef INLINE_DEBUG_RASTER_VBLANK_START
	//debug << "\r\nRGNE1=" << dec << NextEvent_Cycle;
	//debug << " lVBlank=" << lVBlank;
#endif


	// handle interlacing //

	// update scanline number
	lScanline = lNextScanline;
	lNextScanline += 2;
	if ( lNextScanline >= lMaxScanline )
	{
		lNextScanline -= lMaxScanline;
	}


#ifndef DISABLE_FIELD
	// check SMODE2 to see if in interlaced mode or not
	if ( GPURegs0.SMODE2.INTER )
	{
		// Interlaced mode //
		
		// set the field being drawn in CSR
		// 0: even, 1: odd
		GPURegs1.CSR.FIELD = lScanline & 1;
	}
	else
	{
		// NON-Interlaced mode //
		
		// set the field being drawn??
		// maybe it changes per scanline?? or per draw??
		GPURegs1.CSR.FIELD = lScanline & 1;
	}
#endif



	// vblank end is only when the next scanline is less than the just completed scanline
	//if ( lNextScanline < lScanline )
	if ( ! ( lScanline & ~1 ) )
	{
		// End of VBLANK //

#ifndef DISABLE_NFIELD

		// set NFIELD (gets set at VSYNC??)
		GPURegs1.CSR.NFIELD = GPURegs1.CSR.FIELD;

#endif


#ifndef DISABLE_VSYNCE_INT
		// trigger end of vblank interrupt for intc
		// note: this is actually vblank end interrupt, need to rename function
		SetInterrupt_Vsync_End ();
#endif


#ifndef DISABLE_VSINT
		// make sure that bit is clear (enabled) in CSR
		if ( !GPURegs1.CSR.VSINT )
		{
			// check if vblank is masked
			// if bit in IMR register is 1, it means to ignore interrupt. If it is zero, then means to trigger interrupt
			if ( !GPURegs1.IMR.VSMSK )
			{
				// trigger interrupt due to vsync
				SetInterrupt ();
				
			}
			
			// set bit in CSR (vsync event?? or maybe vsync interrupt occurred)
			GPURegs1.CSR.VSINT = 1;

		}	// end if ( !GPURegs1.CSR.VSINT )
#endif


		
#ifdef INLINE_DEBUG_RASTER_VBLANK_END
	debug << "\r\n\r\n***VBLANK END*** " << hex << setw( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << "; BusyCycles=" << dec << BusyCycles << " Scanline_Number=" << lScanline;
#endif


#ifdef ENABLE_SCREEN_DRAW_VBLANK_END
		// update count of frames for debugging
		Frame_Count++;
		
		// draw screen //

		// draw output to program window @ vblank! - if output window is available
		if ( DisplayOutput_Window )
		{
#ifdef USE_TEMPLATES_PS2_DRAWSCREEN
			u64 *inputdata_ptr;
			u64 NumPixels;
			
			if ( GPU::ulThreadedGPU )
			{
				WaitForSingleObject ( ghEvent_PS2GPU_Frame, INFINITE );
				
				// update total elapsed time
				ullTotalElapsedTime += ullElapsedTime;
				
				// reset elapsed time for the next frame
				ullElapsedTime = 0;
			}


			// this actually puts in the code for the command
#ifdef USE_NEW_INPUTFORMAT
			Select_DrawScreen_t ( (u64*) display_inputbuffer, 0 );
#else
			inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
			Select_DrawScreen_t ( inputdata_ptr, 0 );
#endif
			
			
#else

			// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
			Finish ();

			Draw_Screen ();
#endif
			
			if ( DebugWindow_Enabled )
			{
				if ( !GPU::ulThreadedGPU )
				{
					Draw_FrameBuffers ();
				}
			}
		}
#endif

	}	// end if ( ! ( lScanline & ~1 ) )




#ifdef INLINE_DEBUG_RASTER_VBLANK_START
	//debug << " lVBlank=" << lVBlank;
#endif



	// check if this is vblank or time to draw screen
	if ( ( lScanline & ~1 ) == lVBlank )
	{
		// vblank //
		
#ifdef INLINE_DEBUG_RASTER_VBLANK_START
	debug << "\r\n\r\n***VBLANK START*** " << hex << setw( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << "; BusyCycles=" << dec << BusyCycles << " Scanline_Number=" << lScanline;
	debug << " Interlaced?=" << GPURegs0.SMODE2.INTER << " Field=" << GPURegs1.CSR.FIELD << " Nfield=" << GPURegs1.CSR.NFIELD;
	debug << " FBOffset1/2048=" << hex << GPURegs0.DISPFB1.FBP;
	debug << " FBOffset2/2048=" << hex << GPURegs0.DISPFB2.FBP;
	debug << " FBWidth1/64 (for screen output/in pixels)=" << dec << GPURegs0.DISPFB1.FBW;
	debug << " FBWidth2/64 (for screen output/in pixels)=" << dec << GPURegs0.DISPFB2.FBW;
	debug << " FBHeight1-1 (for screen output/in pixels)=" << dec << GPURegs0.DISPLAY1.DH;
	debug << " FBHeight2-1 (for screen output/in pixels)=" << dec << GPURegs0.DISPLAY2.DH;
	debug << " XPixel1=" << dec << GPURegs0.DISPFB1.DBX;
	debug << " YPixel1=" << dec << GPURegs0.DISPFB1.DBY;
	debug << " XPixel2=" << dec << GPURegs0.DISPFB2.DBX;
	debug << " YPixel2=" << dec << GPURegs0.DISPFB2.DBY;
	debug << " PixelFormat0=" << PixelFormat_Names [ GPURegs0.DISPFB1.PSM ];
	debug << " PixelFormat1=" << PixelFormat_Names [ GPURegs0.DISPFB2.PSM ];
	debug << " PMODE=" << hex << GPURegs0.PMODE;
	debug << " EXTWRITE=" << hex << GPURegs0.EXTWRITE;
	debug << " EXTDATA=" << hex << GPURegs0.EXTDATA;
	debug << " EXTBUF=" << hex << GPURegs0.EXTBUF;
#endif


		// handle vblank //

#ifndef DISABLE_VSYNC_INT
		// trigger start of vblank interrupt for intc
		// note: this is actually a start of vblank interrupt, need to rename the function
		SetInterrupt_Vsync_Start ();
#endif

		
#ifdef ENABLE_SCREEN_DRAW_VBLANK_START
		// update count of frames for debugging
		Frame_Count++;
		
		// draw screen //

		// draw output to program window @ vblank! - if output window is available
		if ( DisplayOutput_Window )
		{
#ifdef USE_TEMPLATES_PS2_DRAWSCREEN
			u64 *inputdata_ptr;
			u64 NumPixels;

			
			if ( GPU::ulThreadedGPU )
			{
			//cout << "\nWaiting for previous frame gputargetidx2=" << dec << ulInputBuffer_TargetIndex2 << " gpureadidx=" << ulInputBuffer_ReadIndex;

				//Flush ();
				//SetEvent ( ghEvent_PS2GPU_Update );
				//SetEvent ( VU::ghEvent_PS2VU1_Update );
				WaitForSingleObject ( ghEvent_PS2GPU_Frame, INFINITE );


				// update total elapsed time
				ullTotalElapsedTime += ( ullEndTime - ullStartTime );


				// get the start timer value for the run
				if ( !QueryPerformanceCounter ( (LARGE_INTEGER*) &ullStartTime ) )
				{
					cout << "\nhpsx64: PS2GPUThread: Error returned from QueryPerformanceCounter at StartTimer\n";
				}

				// reset elapsed time for the next frame
				//ullElapsedTime = 0;
			}


			// this actually puts in the code for the command
#ifdef USE_NEW_INPUTFORMAT

			Select_DrawScreen_t ( (u64*) display_inputbuffer, 0 );

#else
			inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
			Select_DrawScreen_t ( inputdata_ptr, 0 );
#endif


#ifdef ENABLE_THREAD_LOCK
	// if the vu1 thread is active, then do a flush on vu1
	// if ONLY the GPU thread is active, then do a flush on gpu
	if ( VU::ulThreadCount )
	{
		// flush vu commands and corresponding gpu data //
		VU::Flush ();
	}
	else if ( ulNumberOfThreads )
	{
		// flush gpu //
		Flush ();
	}
#endif


			
#else

			// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
			//Finish ();

			Draw_Screen ();

#endif	// end #ifdef USE_TEMPLATES_PS2_DRAWSCREEN
			
			if ( DebugWindow_Enabled )
			{
				if ( !GPU::ulThreadedGPU )
				{
					Draw_FrameBuffers ();
				}
			}
		}
#endif
		
		

	}	// end if ( ( lScanline & ~1 ) == lVBlank )

#ifdef INLINE_DEBUG_RASTER_SCANLINE
	debug << "\r\n\r\n***SCANLINE*** " << hex << setw( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << "; BusyCycles=" << dec << BusyCycles << " Scanline=" << lScanline << " llScanlineStart=" << llScanlineStart << " llHBlankStart=" << llHBlankStart << " llNextScanlineStart=" << llNextScanlineStart << " lVBlank=" << lVBlank << " CyclesPerScanline=" << dCyclesPerScanline;
	debug << " Interlaced?=" << GPURegs0.SMODE2.INTER << " Field=" << GPURegs1.CSR.FIELD;
#endif


#ifndef DISABLE_HSINT

	if ( lScanline < lVBlank )
	{
		// make sure that bit is clear (enabled) in CSR
		if ( !GPURegs1.CSR.HSINT )
		{
#ifdef INLINE_DEBUG_HBLANK_INT
	debug << "\r\n***HBLANK INTERRUPT ENABLED***";
#endif

			// check if hblank is masked
			// if bit in IMR register is 1, it means to ignore interrupt. If it is zero, then means to trigger interrupt
			if ( !GPURegs1.IMR.HSMSK )
			{
#ifdef INLINE_DEBUG_HBLANK_INT
	debug << "\r\n***HBLANK INTERRUPT TRIGGERED***";
#endif

				// trigger interrupt due to vsync
				SetInterrupt ();
				
			}
			
			// set bit in CSR (vsync event?? or maybe vsync interrupt occurred)
			GPURegs1.CSR.HSINT = 1;

		}	// end if ( !GPURegs1.CSR.HSINT )

	}
#endif


	// update timers //
	// do this before updating the next event
#ifdef USE_NEW_SYNC_PS2TIMERS
	Timers::_TIMERS->UpdateTimer_Sync ( 0 );
	Timers::_TIMERS->UpdateTimer_Sync ( 1 );
	Timers::_TIMERS->UpdateTimer_Sync ( 2 );
	Timers::_TIMERS->UpdateTimer_Sync ( 3 );
#else
	Timers::_TIMERS->UpdateTimer ( 0 );
	Timers::_TIMERS->UpdateTimer ( 1 );
	Timers::_TIMERS->UpdateTimer ( 2 );
	Timers::_TIMERS->UpdateTimer ( 3 );
#endif
	
	// get the cycle number at start of the next scanline and update associated variables
	Update_NextEvent ();
	
	// update timer events //
#ifdef USE_NEW_SYNC_PS2TIMERS
	Timers::_TIMERS->Get_NextEvent_Sync ( 0 );
	Timers::_TIMERS->Get_NextEvent_Sync ( 1 );
	Timers::_TIMERS->Get_NextEvent_Sync ( 2 );
	Timers::_TIMERS->Get_NextEvent_Sync ( 3 );
#else
	Timers::_TIMERS->Get_NextEvent ( 0 );
	Timers::_TIMERS->Get_NextEvent ( 1 );
	Timers::_TIMERS->Get_NextEvent ( 2 );
	Timers::_TIMERS->Get_NextEvent ( 3 );
#endif
	
}


void GPU::SetNextEvent ( u64 CycleOffset )
{
	NextEvent_Cycle = CycleOffset + *_DebugCycleCount;
	
	Update_NextEventCycle ();
}

void GPU::SetNextEvent_Cycle ( u64 Cycle )
{
	NextEvent_Cycle = Cycle;
	
	Update_NextEventCycle ();
}

void GPU::Update_NextEventCycle ()
{
	//if ( NextEvent_Cycle > *_DebugCycleCount && ( NextEvent_Cycle < *_NextSystemEvent || *_NextSystemEvent <= *_DebugCycleCount ) )
	if ( NextEvent_Cycle < *_NextSystemEvent )
	{
		*_NextSystemEvent = NextEvent_Cycle;
		*_NextEventIdx = NextEvent_Idx;
	}
}





void GPU::Update_NextEvent ()
{
	//u64 CycleOffset1;
	//double dCyclesToNext;
	
	dScanlineStart = dNextScanlineStart;
	dNextScanlineStart += dCyclesPerScanline;
	dHBlankStart = dNextScanlineStart - dHBlankArea_Cycles;
	
	// convert to integers
	llScanlineStart = (u64) dScanlineStart;
	llNextScanlineStart = (u64) dNextScanlineStart;

	if ( lScanline < lVBlank )
	{
		llHBlankStart = (u64) dHBlankStart;
	}
	else
	{
		llHBlankStart = -1ull;
	}
	
	SetNextEvent_Cycle ( llNextScanlineStart );
	
	/*
	dGPU_NextEventCycle += dCyclesPerScanline;
	//iGPU_NextEventCycle += iCyclesPerScanline;
	//dCyclesToNext = (double)(*_DebugCycleCount)
	//CycleOffset1 = (u64) dGPU_NextEventCycle;
	
	NextEvent_Cycle = (u64) dGPU_NextEventCycle;
	
	if ( ( dGPU_NextEventCycle - ( (double) NextEvent_Cycle ) ) > 0.0L ) NextEvent_Cycle++;
	
	//SetNextEvent_Cycle ( iGPU_NextEventCycle );
	Update_NextEventCycle ();
	*/
	
#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::Update_NextEvent CycleOffset=" << dec << dCyclesPerScanline;
#endif
}


void GPU::GetNextEvent ()
{
	//u64 CycleOffset1;	//, CycleOffset2;
	
	// *todo* should also probably combine this stuff with updating the current scanline number and re-calibrating the timers
	lScanline = GetScanline_Number ();
	lNextScanline = lScanline + 2;
	if ( lNextScanline >= lMaxScanline )
	{
		// End of VBLANK //
		lNextScanline -= lMaxScanline;
	}
	
	dScanlineStart = GetScanline_Start ();
	dNextScanlineStart = dScanlineStart + dCyclesPerScanline;
	dHBlankStart = dNextScanlineStart - dHBlankArea_Cycles;
	
	// convert to integers
	llScanlineStart = (u64) dScanlineStart;
	llNextScanlineStart = (u64) dNextScanlineStart;
	llHBlankStart = (u64) dHBlankStart;
	
	// the next gpu event is at the start of the next scanline
	//dGPU_NextEventCycle = dScanlineStart;
	SetNextEvent_Cycle ( llNextScanlineStart );
	
	/*
	dGPU_NextEventCycle = GetCycles_ToNextScanlineStart ();
	//CycleOffset1 = (u64) ceil ( GetCycles_ToNextScanlineStart () );
	CycleOffset1 = (u64) ceil ( dGPU_NextEventCycle );
	
	// need to store cycle number of the next scanline start
	dGPU_NextEventCycle += (double) ( *_DebugCycleCount );
	
	//CycleOffset1 = (u64) ceil ( dGPU_NextEventCycle );

#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::GetNextEvent CycleOffset=" << dec << dCyclesToNext;
#endif

	//if ( CycleOffset1 < CycleOffset2 )
	//{
		// set the vblank as the next event
		SetNextEvent ( CycleOffset1 );
	//}
	//else
	//{
		// set drawing the screen as the next event
		//SetNextEvent ( CycleOffset2 );
	//}
	*/
}


void GPU::GetNextEvent_V ()
{
	u64 CycleOffset1;


#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// *** testing *** get next vsync event
	CycleOffset1 = (u64) ceil ( GetCycles_ToNextVBlank () );
#else
	CycleOffset1 = CEILD ( GetCycles_ToNextVBlank () );
#endif

	
	NextEvent_Cycle_Vsync = *_DebugCycleCount + CycleOffset1;

#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::GetNextEvent CycleOffset=" << dec << dCyclesToNext;
#endif

	//SetNextEvent_V ( CycleOffset1 );
}


u64 GPU::Read ( u32 Address, u64 Mask )
{
	u32 Temp;
	u64 Output = 0;
	u32 lReg;

#ifdef INLINE_DEBUG_READ
	debug << "\r\n\r\nGPU::Read; " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << " Address = " << hex << Address << " Mask=" << Mask;
#endif

	// will set some values here for now
	switch ( Address )
	{
		case GIF_STAT:
		
#ifdef INLINE_DEBUG_READ
	debug << " GPUBusyUntil_Cycle=" << dec << _GPU->BusyUntil_Cycle;
#endif

			// check if the GPU is busy with something
			if ( *_DebugCycleCount < _GPU->BusyUntil_Cycle )
			{
				// set the current path
				/*
				_GPU->GIFRegs.STAT.APATH = _GPU->CurrentPath;
				
				Temp = ( _GPU->FifoSize > 16 ) ? 16 : _GPU->FifoSize;
				
				_GPU->GIFRegs.STAT.FQC = Temp;
				*/
				
				/*
				// request to wait
				// zero for now
				//_GPU->GIFRegs.STAT.P1Q = 1;
				//_GPU->GIFRegs.STAT.P2Q = 1;
				//_GPU->GIFRegs.STAT.P3Q = 1;
				_GPU->GIFRegs.STAT.P1Q = 0;
				_GPU->GIFRegs.STAT.P2Q = 0;
				_GPU->GIFRegs.STAT.P3Q = 0;
				*/
			}
			else
			{
				// fifo must be empty
				_GPU->FifoSize = 0;
				Temp = 0;
				
				/*
				// temporary cosmetic change, need to handle the timing issues for gpu
				if ( !_GPU->GIFRegs.STAT.M3R && !_GPU->GIFRegs.STAT.M3P )
				{
					// path3 is not masked, so processing in path3 is activated
					_GPU->GIFRegs.STAT.FQC = 0;
					//_GPU->GIFRegs.STAT.P3Q = 0;
				}
				
				// clear path
				_GPU->GIFRegs.STAT.APATH = 0;
				*/
				
				/*
				// no request to wait
				_GPU->GIFRegs.STAT.P1Q = 0;
				_GPU->GIFRegs.STAT.P2Q = 0;
				*/
				
				/*
				// not currently outputting data
				_GPU->GIFRegs.STAT.OPH = 0;
				*/
			}
			
			break;
			
		case GPU_CSR:
			_GPU->GPURegs1.CSR.REV = c_ulGS_Revision;
			_GPU->GPURegs1.CSR.ID = c_ulGS_ID;
			
			// test if drawing is complete
			if ( *_DebugCycleCount < _GPU->BusyUntil_Cycle )
			{
				if ( _GPU->FifoSize )
				{
					// fifo is full or close to full ??
					_GPU->GPURegs1.CSR.FIFO = 2;
				}
				else if ( !_GPU->FifoSize )
				{
					// fifo is empty
					_GPU->GPURegs1.CSR.FIFO = 1;
				}
			}
			else
			{
				// fifo is empty
				_GPU->GPURegs1.CSR.FIFO = 1;
			}
			
			// bits 13-14 should be set to 1 ??
			//_GPU->GPURegs1.CSR.Value |= 0x6000;
			// setting bit 14 to 1 means that fifo is empty
			// ***todo*** implement correct fifo status
			//_GPU->GPURegs1.CSR.Value |= 0x4000;
			
			break;
			
		default:
			break;
	}
	
	lReg = ( Address >> 4 ) & 0xf;

	// check if these are GIF Regs or GPU Priveleged Registers
	switch ( Address & 0xf000 )
	{
		// GPU priveleged registers group 0
		case 0x0000:
		
		// and it mirrors again at 0x0400 and 0x0800
		//case 0x0400:
		//case 0x0800:
#ifdef INLINE_DEBUG_READ
			if ( lReg < 15 )
			{
			debug << "; " << _GPU->GPUReg0Names [ lReg ];
			}
#endif

			Output = _GPU->GPURegs0.Regs [ lReg ];
			break;
			
		// GPU priveleged registers group 1
		case 0x1000:
		
		// and it mirrors again at 0x1400 and 0x1800
		//case 0x1400:
		//case 0x1800:
#ifdef INLINE_DEBUG_READ
			if ( lReg < 9 )
			{
			debug << "; " << _GPU->GPUReg1Names [ lReg ];
			}
#endif

			Output = _GPU->GPURegs1.Regs [ lReg ];
			break;
			
		// GIF Registers
		case 0x3000:
#ifdef INLINE_DEBUG_READ
			if ( lReg < 11 )
			{
			debug << "; " << _GPU->GIFRegNames [ lReg ];
			}
#endif

			Output = _GPU->GIFRegs.Regs [ lReg ];
			break;
			
		// GIF FIFO
		case 0x6000:
#ifdef INLINE_DEBUG_READ
			debug << "; GIF_FIFO";
#endif

			// GIF FIFO is WRITE-ONLY //

			break;
	}
	

	
	/*
	switch ( Address )
	{
		case GIF_CTRL:	// 0x10003000
#ifdef INLINE_DEBUG_READ
			debug << "; GIF_CTRL";
#endif

			break;
			
		case GIF_MODE:	// 0x10003010
#ifdef INLINE_DEBUG_READ
			debug << "; GIF_MODE";
#endif

			break;
			
		case GIF_STAT:	// 0x10003020
#ifdef INLINE_DEBUG_READ
			debug << "; GIF_STAT";
#endif

			Output = _GPU->GIF_STAT_Reg.Value;
			break;
			
		case GIF_FIFO:	// 0x10006000
#ifdef INLINE_DEBUG_READ
			debug << "; GIF_FIFO";
#endif

			break;
	}
	*/
	
	
#ifdef INLINE_DEBUG_READ
	debug << "; Output=" << hex << Output;
#endif

	return Output;
}


void GPU::Write ( u32 Address, u64 Data, u64 Mask )
{
	u32 lReg;
	u32 ulTempArray [ 4 ];
	GIFTag1_t Arg0, Arg1;
	
	u64 PreviousValue;
	
#ifdef INLINE_DEBUG_WRITE
	debug << "\r\n\r\nGPU::Write; " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << " Address = " << hex << Address << "; Data = " << Data << " Mask=" << Mask;
#endif

	//if ( ( Mask ) && ( Mask != 0xffffffffffffffffULL ) )
	//{
	//	cout << "\nhps2x64: ALERT: GPU: Write: Mask=" << hex << Mask << " Address=" << Address;
	//}
	
	// apply write mask to non-128-bit writes?
	//if ( Mask )
	//{
	//	Data &= Mask;
	//}
	
	// *todo* alert if write offset for GPU is not zero

	// perform actions before write
	switch ( Address )
	{
		// read-only registers
		case GIF_STAT:
			return;
			break;
			
		default:
			break;
	}

	

	
	
	// get register number being written to
	lReg = ( Address >> 4 ) & 0xf;

	
	// check if these are GIF Regs or GPU Priveleged Registers
	switch ( Address & 0xf000 )
	{
		// GPU priveleged registers group 0
		case 0x0000:
#ifdef INLINE_DEBUG_WRITE
			if ( lReg < 15 )
			{
			debug << "; " << _GPU->GPUReg0Names [ lReg ];
			}
#endif

			_GPU->GPURegs0.Regs [ lReg ] = Data;
			break;
			
		// GPU priveleged registers group 1
		case 0x1000:
#ifdef INLINE_DEBUG_WRITE
			if ( lReg < 9 )
			{
			debug << "; " << _GPU->GPUReg1Names [ lReg ];
			}
#endif

			// handle before write
			switch ( lReg )
			{
				// BUSDIR
				case 0x4:
#ifdef INLINE_DEBUG_WRITE
					debug << "\r\nsetting bus dir.";
					cout << "\nhps2x64: GPU: ALERT: setting bus dir priveleged register.";
#endif

					// if there is going to be change to gpu->mem, and multi-threading gpu, then synchronize
					if ( GPU::ulThreadedGPU )
					{
						if ( ( Data & 1 ) && ( ( _GPU->GIFRegs.STAT.DIR ^ Data ) & 1 ) )
						{
							if ( VU::ulThreadCount )
							{
								// wait for vu thread to finish
								VU::Finish ();

								// if gpu is on it's own thread, then wait for it to finish
								// otherwise it is finished when vu thread is finished
								if ( ulNumberOfThreads )
								{
									// wait for GPU to finish
									Finish ();
								}
							}
							else
							{
								// synchronize with gpu thread
								Finish ();
							}
						}
					}

					// changing direction of bus, also set dir
					_GPU->GIFRegs.STAT.DIR = Data & 0x1;

					// set OPH ??
					_GPU->GIFRegs.STAT.OPH = Data & 0x1;
					
					// check dma
					Dma::_DMA->CheckTransfer ();
				
					break;
			}

			_GPU->GPURegs1.Regs [ lReg ] = Data;
			break;
			
		// GIF Registers
		case 0x3000:
#ifdef INLINE_DEBUG_WRITE
			if ( lReg < 11 )
			{
			debug << "; " << _GPU->GIFRegNames [ lReg ];
			}
#endif

			_GPU->GIFRegs.Regs [ lReg ] = Data;
			break;
			
		// GIF FIFO
		case 0x6000:
#ifdef INLINE_DEBUG_WRITE
			debug << "; GIF_FIFO";
			if ( !Mask ) debug << "; Value=" << hex << ((u32*)Data) [ 0 ] << " " << ((u32*)Data) [ 1 ] << " " << ((u32*)Data) [ 2 ] << " " << ((u32*)Data) [ 3 ];
#endif

			if ( _GPU->CurrentPath != 3 )
			{
				_GPU->lVertexCount = 0;
			}

			// transferring data via path 3
			// ***TODO*** or maybe treat this as a separate path??
			_GPU->CurrentPath = 3;
			
			// if device not busy, then clear fifo size
			if ( *_DebugCycleCount < _GPU->BusyUntil_Cycle )
			{
				_GPU->FifoSize = 0;
			}
			else
			{
				// otherwise, add the data into fifo since device is busy
				_GPU->FifoSize++;
			}

			// make sure write is 128-bit
			if ( !Mask )
			{
				_GPU->GIF_FIFO_Execute ( ((u64*)Data), 2 );
			}
			
			break;
	}

	// perform actions after write
	switch ( Address )
	{
		case GIF_CTRL:
		
			if ( Data & 1 )
			{
				// GIF RESET //

				// reset command written ONLY for GIF FIFO
				
				// ***TODO*** reset entire GIF FIFO
				
				// fifo is empty
				_GPU->GPURegs1.CSR.FIFO = 0x1;
				
				// clear the count of items in the GIF FIFO
				//_GPU->GIFRegs.STAT.FQC = 0;
				// also set GIF_STAT to the initial values
				_GPU->GIFRegs.STAT.Value = 0;
				_GPU->GIFRegs.MODE.Value = 0;
				_GPU->GIFRegs.CTRL.Value = 0;
				_GPU->GIFRegs.TAG0 = 0;
				_GPU->GIFRegs.TAG1 = 0;
				_GPU->GIFRegs.TAG2 = 0;
				_GPU->GIFRegs.TAG3 = 0;
				_GPU->GIFRegs.CNT = 0;
				_GPU->GIFRegs.P3CNT = 0;
				_GPU->GIFRegs.P3TAG = 0;
				
				// reset
				for ( int i = 0; i < c_iNumPaths; i++ )
				{
					_GPU->ulTransferCount [ i ] = 0;
					_GPU->ulTransferSize [ i ] = 0;
				}
			}
			
			// ***TODO*** implement temporary transfer stop
			if ( Data & 0x8 )
			{
				cout << "\nhps2x64: GPU: PSE -> Temporary Transfer Stop!!! Unimplemented!!!\n";
				
				// set PSE in STAT
				_GPU->GIFRegs.STAT.PSE = 1;
			}
			else
			{
				// clear PSE in STAT
				_GPU->GIFRegs.STAT.PSE = 0;
				
				// let dma know that something has changed
				Dma::_DMA->CheckTransfer ();
			}
			
			break;
			
		case GIF_MODE:
		
			PreviousValue = _GPU->GIFRegs.STAT.M3R;
		
			// set M3R (bit 0) to gif_stat M3R (bit 0)
			_GPU->GIFRegs.STAT.M3R = Data & 1;
			
			// set IMT
			_GPU->GIFRegs.STAT.IMT = ( Data >> 2 ) & 1;
			
			if ( PreviousValue )
			{
				// check for a transition from 1 to 0
				if ( !_GPU->GIFRegs.STAT.M3R && !_GPU->GIFRegs.STAT.M3P )
				{
					// path 3 mask is being disabled //
#ifdef INLINE_DEBUG_WRITE
	debug << "\r\n*** PATH3 BEING UN-MASKED VIA GPU ***\r\n";
#endif
#ifdef VERBOSE_MSKPATH3
	cout << "\n*** PATH3 BEING UN-MASKED VIA GPU ***\n";
#endif

					// re-check if vif can run
					VU::_VU[ 1 ]->VifStopped = 0;
					
					// restart dma#2
					//Dma::_DMA->Transfer ( 2 );
					Dma::_DMA->CheckTransfer ();
				}
			}
			
			if ( !PreviousValue )
			{
				if ( _GPU->GIFRegs.STAT.M3R )
				{
					// transfer is being masked //
#ifdef INLINE_DEBUG_WRITE
	debug << "\r\n*** PATH3 BEING MASKED VIA GPU ***\r\n";
#endif
#ifdef VERBOSE_MSKPATH3
	cout << "\n*** PATH3 BEING MASKED VIA GPU ***\n";
#endif

					// check if path3 is in progress while vif is running
					if ( ( Dma::pRegData [ 1 ]->CHCR.STR ) && ( Dma::pRegData [ 2 ]->CHCR.STR ) && ( !GPU::_GPU->EndOfPacket [ 3 ] ) )
					{
						// if so, stop vif until path3 is masked
						VU::_VU[ 1 ]->VifStopped = 1;
					}

				}
			}
			
			break;
			
		case GIF_FIFO:
		
			// increment number of items in GIF FIFO
			_GPU->GIFRegs.STAT.FQC++;
			
			// assume data was transferred for now
			_GPU->GIFRegs.STAT.FQC = 0;
			
			break;
			
		case GPU_CSR:
#ifdef INLINE_DEBUG_WRITE
			debug << "; GPU_CSR";
#endif
		
			if ( _GPU->GPURegs1.CSR.RESET )
			{
				// GPU RESET //

#ifdef INLINE_DEBUG_WRITE
			debug << "; GPU-SYSTEM-RESET";
#endif

				// clear gpu ram
				//memset ( _GPU->RAM8, 0, GPU::c_iRAM_Size );


				// clear reset flag??
				// or set it?
				_GPU->GPURegs1.CSR.Value = 0;
				//_GPU->GPURegs1.CSR.RESET = 1;

				// fifo is empty
				_GPU->GPURegs1.CSR.FIFO = 0x1;

				_GPU->GPURegs1.CSR.REV = c_ulGS_Revision;
				_GPU->GPURegs1.CSR.ID = c_ulGS_ID;

				// interrupt mask is set to disable all interrupts when gpu is reset
				_GPU->GPURegs1.IMR.Value = 0x7f00;

				/*
				_GPU->GPURegs1.BUSDIR = 0;
				_GPU->GPURegs1.SIGLBLID.Value = 0;

				
				// clear the count of items in the GIF FIFO ??
				_GPU->GIFRegs.STAT.FQC = 0;

				_GPU->GPURegs0.PMODE = 4;
				_GPU->GPURegs0.SMODE1 = 0;
				_GPU->GPURegs0.SMODE2.Value = 0;
				_GPU->GPURegs0.SRFSH = 0;
				_GPU->GPURegs0.SYNCH1 = 0;
				_GPU->GPURegs0.SYNCH2 = 0;
				_GPU->GPURegs0.SYNCV = 0;
				_GPU->GPURegs0.DISPFB1.Value = 0;
				_GPU->GPURegs0.DISPLAY1.Value = 0;
				_GPU->GPURegs0.DISPFB2.Value = 0;
				_GPU->GPURegs0.DISPLAY2.Value = 0;
				_GPU->GPURegs0.EXTBUF = 0;
				_GPU->GPURegs0.EXTDATA = 0;
				_GPU->GPURegs0.EXTWRITE = 0;
				_GPU->GPURegs0.BGCOLOR = 0;

				// reset general purpose registers
				for ( int i = 0; i < c_iGPURegsGp_Count; i++ )
				{
					_GPU->GPURegsGp.Regs [ i ] = 0;
				}
				*/

				// reset any internal variables
				for ( int i = 0; i < c_iNumPaths; i++ )
				{
					_GPU->ulTransferCount [ i ] = 0;
					_GPU->ulTransferSize [ i ] = 0;
				}

				// done with reset
				//return;
			}
			
			// clear any interrupts for set bits in CSR (bits 0-4)
			_GPU->GPURegs1.CSR.Value = _GPU->GPURegs1.CSR.Value & ~( Data & 0x1f );
			
			break;
			
		default:
			break;
	}

	/*
	switch ( Address )
	{
		case GIF_CTRL:	// 0x10003000
#ifdef INLINE_DEBUG_WRITE
			debug << "; GIF_CTRL";
#endif

			// check for reset
			if ( Data & 1 )
			{
				// reset command written
				
				// clear the count of items in the GIF FIFO
				_GPU->GIF_STAT_Reg.FQC = 0;
			}

			break;
			
		case GIF_MODE:	// 0x10003010
#ifdef INLINE_DEBUG_WRITE
			debug << "; GIF_MODE";
#endif

			break;
			
		case GIF_STAT:	// 0x10003020
#ifdef INLINE_DEBUG_WRITE
			debug << "; GIF_STAT";
#endif

			break;
			
		case GIF_FIFO:	// 0x10006000
#ifdef INLINE_DEBUG_WRITE
			debug << "; GIF_FIFO";
			if ( !Mask ) debug << "; Value=" << ((u64*)Data) [ 0 ] << " " << ((u64*)Data) [ 1 ];
#endif

			// increment number of items in GIF FIFO
			_GPU->GIF_STAT_Reg.FQC++;
			
			// assume data was transferred
			_GPU->GIF_STAT_Reg.FQC = 0;

			break;
	}
	*/
	
}


// returns count of the remainder of data
u32 GPU::GIF_FIFO_Execute ( u64* pData64, u32 Count64 )
{
	// ***todo*** use pointers instead as input
	
	u64 ulDestReg;
	u32 TransferCount64, TransferRemaining64;
	u64 Arg0, Arg1;

	u64 ulRunCount, ulRunLeft, ulSendCount;
	u64 ulRegList;
	u64 iRegListIdx;
	
	u64 *inputdata_ptr;
	
	u64 TempArr [ 2 ];
	
	// loop while there is data to transfer and transfer is in progress
	while ( Count64 )
	{
	
	if ( !ulTransferCount [ CurrentPath ] )
	{
		// new packet, new primitive...
		// ...or not
		//StartPrimitive ();

		// next packet is start of packet
		bNextPacketHighPriority = false;
		
		GIFTag0 [ CurrentPath ].Value = *pData64++;
		
		GIFTag1 [ CurrentPath ].Value = *pData64++;
	
#ifdef INLINE_DEBUG_FIFO
			debug << "; GIFTag0 [ CurrentPath ]=" << hex << GIFTag0 [ CurrentPath ].Value;
			debug << "; GIFTag1 [ CurrentPath ]=" << hex << GIFTag1 [ CurrentPath ].Value;
#endif


		// set registers
		GIFRegs.TAG0 = (u32) ( GIFTag0 [ CurrentPath ].Value );
		GIFRegs.TAG1 = (u32) ( GIFTag0 [ CurrentPath ].Value >> 32 );
		GIFRegs.TAG2 = (u32) ( GIFTag1 [ CurrentPath ].Value );
		GIFRegs.TAG3 = (u32) ( GIFTag1 [ CurrentPath ].Value >> 32 );

		// check if packed, reglist, or Image transfer
		
		// get the number of registers in list
		ulRegCount [ CurrentPath ] = 0;
		
		// get the number of transfers to expect
		ulLoopCount [ CurrentPath ] = 0;
		
		// set num regs
		ulNumRegs [ CurrentPath ] = ( ( !GIFTag0 [ CurrentPath ].REGS ) ? ( 16 ) : ( GIFTag0 [ CurrentPath ].REGS ) );
		
		// get the number of data packets in dwords (64-bit) for reglist mode, qwords (128-bit) for other modes
		switch ( GIFTag0 [ CurrentPath ].FLG )
		{
			// PACKED
			case 0:
#ifdef INLINE_DEBUG_FIFO
			debug << "; PACKED[START]";
#endif

				// check if should store PRIM value (Packed mode only)
				if ( GIFTag0 [ CurrentPath ].PRE )
				{
#ifdef INLINE_DEBUG_FIFO
					debug << "; PRE";
					debug << " PRIM=" << hex << GIFTag0 [ CurrentPath ].PRIM;
#endif

					// start of the packet is here
					// this data can be interrupted, it's the data after that possibly can't
					//bNextPacketHighPriority = true;

					if ( !GPU::ulThreadedGPU )
					{
						//GPURegsGp.PRIM.Value = GIFTag0 [ CurrentPath ].PRIM;
						WriteReg ( PRIM, GIFTag0 [ CurrentPath ].PRIM );
					}
					else
					{
#ifdef USE_NEW_INPUTFORMAT

						// cycle#
						inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = *_DebugCycleCount;
						ulInputBuffer_WriteIndex++;
						inputdata_ptr = (u64*) & ( inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] );
						// put in the command for 128-bit registers
						((u16*)inputdata_ptr) [ 0 ] = 9;

#ifdef ENABLE_PACKET_PRIORITY
						// if this is linked packet, then can't be interrupted
						if ( bNextPacketHighPriority )
						{
							((u16*)inputdata_ptr) [ 0 ] |= 0x10;
						}

						// packet data after this is linked and can't be interrupted
						bNextPacketHighPriority = true;
#endif
						
						// put in the count of 128-bit registers
						((u16*)inputdata_ptr) [ 1 ] = 1;
						
						// put in the path
						((u16*)inputdata_ptr) [ 2 ] = CurrentPath;
						ulInputBuffer_WriteIndex++;
						// put in the list of registers
						inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = 0;
						ulInputBuffer_WriteIndex++;
						// put in the prim value
						inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = GIFTag0 [ CurrentPath ].PRIM;
						ulInputBuffer_WriteIndex++;
#else
						// complete the data output
						inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
						
						// put in the command for 128-bit registers
						((u16*)inputdata_ptr) [ 60 ] = 8;

						// put in the count of 128-bit registers
						((u16*)inputdata_ptr) [ 61 ] = 1;
						
						// put in the path
						((u16*)inputdata_ptr) [ 62 ] = CurrentPath;
						
						// put in the cycle#
						inputdata_ptr [ 14 ] = *_DebugCycleCount;

						// put in the list of registers
						inputdata_ptr [ 13 ] = 0;
						
						// put in the prim value
						inputdata_ptr [ 0 ] = GIFTag0 [ CurrentPath ].PRIM;
						
						// schedule packet to send to the other thread
						ulInputBuffer_WriteIndex++;
#endif

						if ( ( ulInputBuffer_WriteIndex - ulInputBuffer_ReadIndex ) > c_ulInputBuffer_Size )
						{
							cout << "\nhps2x64: GPU: input buffer overrun!!!";
						}

					}	// end else if ( !GPU::ulThreadedGPU )

				}	// end if ( GIFTag0 [ CurrentPath ].PRE )
				
				// transfer size in 64-bit units
				ulTransferSize [ CurrentPath ] = ( GIFTag0 [ CurrentPath ].NLOOP * ulNumRegs [ CurrentPath ] ) << 1;
				break;
			
			// REGLIST
			case 1:
#ifdef INLINE_DEBUG_FIFO
			debug << "; REGLIST[START]";
#endif

				ulTransferSize [ CurrentPath ] = ( GIFTag0 [ CurrentPath ].NLOOP * ulNumRegs [ CurrentPath ] );
				break;
				
			// Image/disabled
			case 2:
#ifdef INLINE_DEBUG_FIFO
			debug << "; IMAGE/DISABLED[START]";
#endif

				ulTransferSize [ CurrentPath ] = ( GIFTag0 [ CurrentPath ].NLOOP ) << 1;
				break;
		}
		
#ifdef INLINE_DEBUG_FIFO
			debug << "; TransferSize=" << hex << ulTransferSize [ CurrentPath ];
#endif

		// include the GIF Tag in TransferSize
		ulTransferSize [ CurrentPath ] += 2;
		
#ifdef INLINE_DEBUG_FIFO
			debug << "; TransferSize(+GIFTAG)=" << hex << ulTransferSize [ CurrentPath ];
#endif

//#ifdef INLINE_DEBUG_INVALID
//	if ( ulTransferSize [ CurrentPath ] > 10000 )
//	{
//		cout << dec << "\nulTransferSize [ CurrentPath ]>10000. Cycle#" << *_DebugCycleCount;
//		debug << dec << "\r\nPossible Error: ulTransferSize [ CurrentPath ]>10000. Cycle#" << *_DebugCycleCount;
//	}
//#endif


		// path is currently transferring
		if ( ulTransferSize [ CurrentPath ] )
		{
			// need to know that path is currently transferring data, so it does not get interrupted by a higher priority path
			PacketInProgress [ CurrentPath ] = 1;
		}
		
		// update transfer count
		ulTransferCount [ CurrentPath ] += 2;
		
		Count64 -= 2;
	}
	else
	{
		// check transfer mode - packed, reglist, image?
		switch ( GIFTag0 [ CurrentPath ].FLG )
		{
			// PACKED (128-bit values)
			case 0:
#ifdef INLINE_DEBUG_FIFO
			debug << "; PACKED";
			debug << " Size=" << dec << ulTransferSize [ CurrentPath ];
			debug << " Count=" << dec << ulTransferCount [ CurrentPath ];
			debug << "; GIFTag0 [ CurrentPath ]=" << hex << GIFTag0 [ CurrentPath ].Value;
			debug << "; GIFTag1 [ CurrentPath ]=" << hex << GIFTag1 [ CurrentPath ].Value;
#endif

				
				ulRunLeft = ulTransferSize [ CurrentPath ] - ulTransferCount [ CurrentPath ];
				
				// get maximum amount of data that can be sent this run
				ulRunCount = ( Count64 < ulRunLeft ) ? Count64 : ulRunLeft;
				
#ifdef USE_NEW_INPUTFORMAT
				ulRunCount = ( ulRunCount < 32 ) ? ulRunCount : 32;
#else
				// do one packet at a time if multi-threading
				ulRunCount = ( ulRunCount < 12 ) ? ulRunCount : 12;
#endif
				
				// clear list of registers
				ulRegList = 0;
				
				// init count of elements to send
				ulSendCount = 0;
			
#ifdef USE_NEW_INPUTFORMAT

				// cycle#
				inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = *_DebugCycleCount;
				ulInputBuffer_WriteIndex++;

				inputdata_ptr = (u64*) & ( inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] );
				// put in the command for 128-bit registers
				((u16*)inputdata_ptr) [ 0 ] = 8;

#ifdef ENABLE_PACKET_PRIORITY
				// if this is linked packet, then can't be interrupted
				if ( bNextPacketHighPriority )
				{
					((u16*)inputdata_ptr) [ 0 ] |= 0x10;
				}

				// packet data after this is linked and can't be interrupted
				bNextPacketHighPriority = true;
#endif

				// put in flag to reset vertex or not
				((u16*)inputdata_ptr) [ 3 ] = lVertexFlag;
				lVertexFlag = 0;

				// put in the count of 128-bit registers
				((u16*)inputdata_ptr) [ 1 ] = ulRunCount >> 1;
				
				// put in the path
				((u16*)inputdata_ptr) [ 2 ] = CurrentPath;
				ulInputBuffer_WriteIndex++;

				// put in the list of registers
				iRegListIdx = ulInputBuffer_WriteIndex;
				ulInputBuffer_WriteIndex++;
				// put in the data
#else
				// start ptr to output data
				inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
#endif
				
				for ( int i = 0; i < ulRunCount; i += 2 )
				{
					// get the register to send to
					ulDestReg = ( GIFTag1 [ CurrentPath ].Value >> ( ulRegCount [ CurrentPath ] << 2 ) ) & 0xf;
					
					// update reg count
					ulRegCount [ CurrentPath ]++;
					
					// if greater or equal to number of registers, reset
					if ( ulRegCount [ CurrentPath ] >= ulNumRegs [ CurrentPath ] ) ulRegCount [ CurrentPath ] = 0;
					
					Arg0 = pData64 [ 0 ];
					Arg1 = pData64 [ 1 ];
					
					// make sure it is not a NOP
					if ( ulDestReg != 0xf )
					{
						if ( !GPU::ulThreadedGPU )
						{
							
							WriteReg_Packed ( ulDestReg, Arg0, Arg1, 0 );
						}
						else
						{
							// check if the register is ADDR+DATA
							// if so, check if it is finish, signal, or label (these should be handled on this thread) (or trxdir?)
							if ( ( ulDestReg == 0xe ) &&  ( ( ( Arg1 & 0xff ) == 0x53 ) || ( ( Arg1 & 0xff ) >= 0x60 ) ) )
							{
								WriteReg_Packed ( ulDestReg, Arg0, Arg1, 0 );
							}
							
							if ( ( ulDestReg != 0xe ) || ( ( Arg1 & 0xff ) < 0x60 ) )
							{
#ifdef USE_NEW_INPUTFORMAT
								inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = Arg0;
								ulInputBuffer_WriteIndex++;
								inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = Arg1;
								ulInputBuffer_WriteIndex++;
#else
								// send the data to the other thread
								*inputdata_ptr++ = Arg0;
								*inputdata_ptr++ = Arg1;
#endif
								
								// put in the register
								//ulRegList <<= 4;
								ulDestReg <<= ( ulSendCount << 2 );
								ulRegList |= ulDestReg;
								
								
								ulSendCount++;
							}
							
							
						}
					}
					
					pData64 += 2;
					
				}
				
				if ( GPU::ulThreadedGPU && ulSendCount )
				{
#ifdef USE_NEW_INPUTFORMAT
					inputdata_ptr = (u64*) & ( inputdata [ ( (iRegListIdx-1) & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
					((u16*)inputdata_ptr) [ 1 ] = ulSendCount;
					inputdata [ iRegListIdx & c_ulInputBuffer_Mask ] = ulRegList;
#else
					// complete the data output
					inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
					
					// put in the command for 128-bit registers
					((u16*)inputdata_ptr) [ 60 ] = 8;

#ifdef ENABLE_PACKET_PRIORITY
					// if this is linked packet, then can't be interrupted
					if ( bNextPacketHighPriority )
					{
						((u16*)inputdata_ptr) [ 60 ] |= 0x10;
					}

					// packet data after this is linked and can't be interrupted
					bNextPacketHighPriority = true;
#endif
					
					// put in the count of 128-bit registers
					((u16*)inputdata_ptr) [ 61 ] = ulSendCount;
					
					// put in the path
					((u16*)inputdata_ptr) [ 62 ] = CurrentPath;
					
					// put in flag to reset vertex or not
					((u16*)inputdata_ptr) [ 63 ] = lVertexFlag;
					lVertexFlag = 0;
					
					// put in the cycle#
					inputdata_ptr [ 14 ] = *_DebugCycleCount;

					// put in the list of registers
					inputdata_ptr [ 13 ] = ulRegList;

//cout << "\nPREPARING: 128-BIT REGS: IDX=" << dec << ulInputBuffer_WriteIndex << " COUNT=" << ulSendCount << " CMD=" << ( inputdata_ptr [ 15 ] & 0xf );

					// schedule packet to send to the other thread
					ulInputBuffer_WriteIndex++;
#endif

					if ( ( ulInputBuffer_WriteIndex - ulInputBuffer_ReadIndex ) > c_ulInputBuffer_Size )
					{
						cout << "\nhps2x64: GPU: input buffer overrun!!!";
					}
				}
#ifdef USE_NEW_INPUTFORMAT
				else
				{
					// didn't send anything
					ulInputBuffer_WriteIndex -= 3;
				}
#endif
				
				// x 128-bit values transferred
				ulTransferCount [ CurrentPath ] += ulRunCount;
				
				Count64 -= ulRunCount;
				
				break;
			
			// REGLIST (64-bit values)
			case 1:
#ifdef INLINE_DEBUG_FIFO
			debug << "; REGLIST";
#endif
			
				ulRunLeft = ulTransferSize [ CurrentPath ] - ulTransferCount [ CurrentPath ];
				
				// get maximum amount of data that can be sent this run
				ulRunCount = ( Count64 < ulRunLeft ) ? Count64 : ulRunLeft;
				
				// do one packet at a time if multi-threading
#ifdef USE_NEW_INPUTFORMAT
				ulRunCount = ( ulRunCount < 16 ) ? ulRunCount : 16;
#else
				ulRunCount = ( ulRunCount < 12 ) ? ulRunCount : 12;
#endif

				// clear list of registers
				ulRegList = 0;
				
				// clear count of registers sent
				ulSendCount = 0;
			
#ifdef USE_NEW_INPUTFORMAT

				// cycle#
				inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = *_DebugCycleCount;
				ulInputBuffer_WriteIndex++;

				inputdata_ptr = (u64*) & ( inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] );
				// put in the command for 128-bit registers
				((u16*)inputdata_ptr) [ 0 ] = 9;

#ifdef ENABLE_PACKET_PRIORITY
				// if this is linked packet, then can't be interrupted
				if ( bNextPacketHighPriority )
				{
					((u16*)inputdata_ptr) [ 0 ] |= 0x10;
				}

				// packet data after this is linked and can't be interrupted
				bNextPacketHighPriority = true;
#endif

				// put in flag to reset vertex or not
				((u16*)inputdata_ptr) [ 3 ] = lVertexFlag;
				lVertexFlag = 0;

				// put in the count of 128-bit registers
				((u16*)inputdata_ptr) [ 1 ] = ulRunCount;
				
				// put in the path
				((u16*)inputdata_ptr) [ 2 ] = CurrentPath;
				ulInputBuffer_WriteIndex++;

				// put in the list of registers
				iRegListIdx = ulInputBuffer_WriteIndex;
				ulInputBuffer_WriteIndex++;
				// put in the data
#else
				// start ptr to output data
				inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
#endif
				
				for ( int i = 0; i < ulRunCount; i += 1 )
				{
				
					// get the register to send to
					ulDestReg = ( GIFTag1 [ CurrentPath ].Value >> ( ulRegCount [ CurrentPath ] << 2 ) ) & 0xf;
					
					// update reg count
					ulRegCount [ CurrentPath ]++;
					
					// if greater or equal to number of registers, reset
					if ( ulRegCount [ CurrentPath ] >= ulNumRegs [ CurrentPath ] ) ulRegCount [ CurrentPath ] = 0;
					
					// get the data element
					Arg0 = *pData64;
					//Arg0 = pData64 [ i ];
					
					// send value to register
					//if ( ulDestReg >= 0xe )
					//{
						// RESERVED
						// NOP
						//case 0xe:
						//case 0xf:
#ifdef INLINE_DEBUG_FIFO
			debug << "; NOP";
#endif

							// NOP just means the data was not output
							// probably still need to advance to the next data element and count as transfer?
							
							
							//break;
					//}
					//else
					//{
							
						//default:
#ifdef INLINE_DEBUG_FIFO
			if ( ulDestReg < c_iGPURegsGp_Count )
			{
			debug << "; " << GPURegsGp_Names [ ulDestReg ];
			}
#endif

							if ( !GPU::ulThreadedGPU )
							{
								//WriteReg ( ulDestReg, pData64 [ 0 ] );
								WriteReg ( ulDestReg, Arg0 );
							}
							else
							{
#ifdef USE_NEW_INPUTFORMAT
								inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = Arg0;
								ulInputBuffer_WriteIndex++;
#else
								*inputdata_ptr++ = Arg0;
#endif
								
								// put in the register
								//ulRegList <<= 4;
								ulDestReg <<= ( ulSendCount << 2 );
								ulRegList |= ulDestReg;
								
								ulSendCount++;
							}
							
							pData64++;
							
							//break;
					//}
					
				}
				
				
				if ( GPU::ulThreadedGPU && ulSendCount )
				{
#ifdef USE_NEW_INPUTFORMAT
					inputdata_ptr = (u64*) & ( inputdata [ ( (iRegListIdx-1) & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
					((u16*)inputdata_ptr) [ 1 ] = ulSendCount;
					inputdata [ iRegListIdx & c_ulInputBuffer_Mask ] = ulRegList;
#else
					// complete the data output
					inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
					
					// put in the command for 64-bit registers
					((u16*)inputdata_ptr) [ 60 ] = 9;
					
#ifdef ENABLE_PACKET_PRIORITY
					// if this is linked packet, then can't be interrupted
					if ( bNextPacketHighPriority )
					{
						((u16*)inputdata_ptr) [ 60 ] |= 0x10;
					}

					// packet data after this is linked and can't be interrupted
					bNextPacketHighPriority = true;
#endif

					// put in the count of 128-bit registers
					((u16*)inputdata_ptr) [ 61 ] = ulSendCount;
					
					// put in the path
					((u16*)inputdata_ptr) [ 62 ] = CurrentPath;
					
					// put in flag to reset vertex or not
					((u16*)inputdata_ptr) [ 63 ] = lVertexFlag;
					lVertexFlag = 0;
					
					// put in the cycle#
					inputdata_ptr [ 14 ] = *_DebugCycleCount;

					// put in the list of registers
					inputdata_ptr [ 13 ] = ulRegList;
					
//cout << "\nPREPARING: 64-BIT REGS: IDX=" << dec << ulInputBuffer_WriteIndex << " COUNT=" << ulSendCount << " CMD=" << ( inputdata_ptr [ 15 ] & 0xf );

					// schedule packet to send to the other thread
					ulInputBuffer_WriteIndex++;
#endif

					if ( ( ulInputBuffer_WriteIndex - ulInputBuffer_ReadIndex ) > c_ulInputBuffer_Size )
					{
						cout << "\nhps2x64: GPU: input buffer overrun!!!";
					}
				}
#ifdef USE_NEW_INPUTFORMAT
				else
				{
					// didn't send anything
					ulInputBuffer_WriteIndex -= 3;
				}
#endif
				
				// run count should be for 2 at a time
				ulRunCount = ( ulRunCount + 1 ) & ~1;
				
				// two 64-bit values transferred
				//ulTransferCount [ CurrentPath ] += 2;
				ulTransferCount [ CurrentPath ] += ulRunCount;
				
				//Count64 -= 2;
				Count64 -= ulRunCount;
				
				break;
			
			// IMAGE/disabled
			default:
#ifdef INLINE_DEBUG_IMAGE
			debug << "; IMAGE/DISABLED";
#endif
			
#ifdef INLINE_DEBUG_XFER
	debug << dec << " DOff32=" << XferDstOffset32 << " X=" << XferX << " Y=" << XferY << " DX=" << XferDstX << " DY=" << XferDstY << " XferWidth=" << XferWidth << " XferHeight=" << XferHeight << " BufWidth=" << XferDstBufWidth;
#endif

				ulRunLeft = ulTransferSize [ CurrentPath ] - ulTransferCount [ CurrentPath ];
				
				// get maximum amount of data that can be sent this run
				ulRunCount = ( Count64 < ulRunLeft ) ? Count64 : ulRunLeft;
				
				// do one packet at a time if multi-threading
#ifdef USE_NEW_INPUTFORMAT
				ulRunCount = ( ulRunCount < 32 ) ? ulRunCount : 32;
#else
				ulRunCount = ( ulRunCount < 14 ) ? ulRunCount : 14;
#endif
				
				if ( GPU::ulThreadedGPU )
				{
					// clear count of registers sent
					ulSendCount = 0;
				
#ifdef USE_NEW_INPUTFORMAT
					// cycle#
					inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = *_DebugCycleCount;
					ulInputBuffer_WriteIndex++;

					inputdata_ptr = (u64*) & ( inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] );
					// put in the command for 128-bit registers
					((u16*)inputdata_ptr) [ 0 ] = 10;

#ifdef ENABLE_PACKET_PRIORITY
					// if this is linked packet, then can't be interrupted
					if ( bNextPacketHighPriority )
					{
						((u16*)inputdata_ptr) [ 0 ] |= 0x10;
					}

					if ( ( CurrentPath != 3 ) || !_GPU->GIFRegs.STAT.IMT )
					{
						bNextPacketHighPriority = true;
					}
					else
					{
						bNextPacketHighPriority = false;
					}
#endif

					// put in flag to reset vertex or not
					((u16*)inputdata_ptr) [ 3 ] = lVertexFlag;
					lVertexFlag = 0;

					// put in the count of 64-bit values
					((u16*)inputdata_ptr) [ 1 ] = ulRunCount;
					
					// put in the path
					((u16*)inputdata_ptr) [ 2 ] = CurrentPath;
					ulInputBuffer_WriteIndex++;

					// put in the list of registers
					//iRegListIdx = ulInputBuffer_WriteIndex;
					//ulInputBuffer_WriteIndex++;
					// put in the data
#else
					// start ptr to output data
					inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
#endif

					for ( int i = 0; i < ulRunCount; i += 1 )
					{
						// send the data to the other thread
#ifdef USE_NEW_INPUTFORMAT
						inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = *pData64;
						ulInputBuffer_WriteIndex++;
#else
						*inputdata_ptr++ = *pData64;
#endif
						
						pData64++;
						ulSendCount++;
					}
				
					if ( GPU::ulThreadedGPU && ulSendCount )
					{
#ifdef USE_NEW_INPUTFORMAT
#else
						// complete the data output
						inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
						
						// put in the command for 128-bit registers
						((u16*)inputdata_ptr) [ 60 ] = 10;
						
#ifdef ENABLE_PACKET_PRIORITY
						// if this is linked packet, then can't be interrupted
						if ( bNextPacketHighPriority )
						{
							((u16*)inputdata_ptr) [ 60 ] |= 0x10;
						}

						// packet data after this is linked and can't be interrupted under certain conditions
						if ( ( CurrentPath != 3 ) || !_GPU->GIFRegs.STAT.IMT )
						{
							bNextPacketHighPriority = true;
						}
						else
						{
							bNextPacketHighPriority = false;
						}
#endif

						// put in the count of 128-bit registers
						((u16*)inputdata_ptr) [ 61 ] = ulSendCount;
						
						// put in the path
						((u16*)inputdata_ptr) [ 62 ] = CurrentPath;

						// put in flag to reset vertex or not
						((u16*)inputdata_ptr) [ 63 ] = lVertexFlag;
						lVertexFlag = 0;
						
						// put in the cycle#
						inputdata_ptr [ 14 ] = *_DebugCycleCount;
						
//cout << "\nPREPARING: IMAGE DATA: IDX=" << dec << ulInputBuffer_WriteIndex << " COUNT=" << ulSendCount << " CMD=" << ( inputdata_ptr [ 15 ] & 0xf );

						// schedule packet to send to the other thread
						ulInputBuffer_WriteIndex++;
#endif

						if ( ( ulInputBuffer_WriteIndex - ulInputBuffer_ReadIndex ) > c_ulInputBuffer_Size )
						{
							cout << "\nhps2x64: GPU: input buffer overrun!!!";
						}
					}
#ifdef USE_NEW_INPUTFORMAT
					else
					{
						// didn't send anything
						ulInputBuffer_WriteIndex -= 2;
					}
#endif
				}
				else
				{
					TransferDataIn32_DS ( (u32*) pData64, ulRunCount << 1 );
					
					// update the pointer
					pData64 += ulRunCount;
				}
				
				// two 64-bit values transferred
				//ulTransferCount [ CurrentPath ] += 2;
				ulTransferCount [ CurrentPath ] += ulRunCount;
				
				//Count64 -= 2;
				Count64 -= ulRunCount;

				break;
		}
		
	}
	
	// if greater than number of items to transfer, then done
	if ( ulTransferCount [ CurrentPath ] >= ulTransferSize [ CurrentPath ] )
	{
		ulTransferCount [ CurrentPath ] = 0;
		ulTransferSize [ CurrentPath ] = 0;
		
		// path no longer has a packet in progress, so can be interrupted
		PacketInProgress [ CurrentPath ] = 0;
		
		// this is the end of the tag
		Tag_Done = 1;
		
		// check if path1 transfer is complete (End of packet)
		if ( GIFTag0 [ CurrentPath ].EOP )
		{
			EndOfPacket [ CurrentPath ] = 1;
			
			// if end of packet and path 3, then re-enable vif1 if needed
			if ( ( CurrentPath == 3 ) && ( VU::_VU[ 1 ]->VifStopped ) )
			{
				VU::_VU[ 1 ]->VifStopped = 0;
				Dma::_DMA->CheckTransfer ();
			}
		}
		
		// the next packet is new and could be interrupted
		bNextPacketHighPriority = false;

		// *new* return the remaining count of 64-bit elements
		return Count64;
	}
	
	}	// while ( Count64 )
		
	// done
	return 0;
}


void GPU::WriteReg_Packed ( u32 lIndex, u64 ull0, u64 ull1, u32 ulThreadNum )	//u64* pValue )
{
	u64 Data;
	
	// make sure index is within range
	if ( lIndex >= c_iGPURegsGp_Count )
	{
#ifdef INLINE_DEBUG_INVALID
	debug << dec << "\r\nWriteReg index outside range. lIndex=" << lIndex << " Cycle#" << *_DebugCycleCount;
#endif

#ifdef COUT_INDEX_RANGE
		cout << "\nhps2x64 ALERT: GPU: WriteReg index outside range.\n";
#endif

		return;
	}
	
	switch ( lIndex )
	{
		// PRIM
		case 0:
			Data = ull0;
			
			//WriteReg ( 0, pValue [ 0 ] );
			//WriteReg ( 0, ull0 );
			break;
			
		// RGBAQ
		case 1:
			//Data = ( ull0 & 0xff ) | ( ( ull0 >> 32 ) & 0xff ) | ( ( ull1 & 0xff ) << 16 ) | ( ( ull1 >> 8 ) & 0xff000000 );
			Data = ( ull0 & 0xff ) | ( ( ull0 >> 24 ) & 0xff00 ) | ( ( ull1 & 0xff ) << 16 ) | ( ( ull1 >> 8 ) & 0xff000000ULL );
			
			// ***TODO*** add in Q value
			Data |= ( Internal_Q << 32 );
			
			//WriteReg ( 1, Data );
			
			break;
			
		// ST
		case 2:
			//Data = pValue [ 0 ];
			Data = ull0;
			
			// save Q value
			//Internal_Q = pValue [ 1 ];
			Internal_Q = ull1;
			
			// don't know if there is a guarantee in which order ST or RGBAQ get executed in
			// so will also set Q of RGBAQ register here
			//GPURegsGp.RGBAQ.Q = Internal_Q;
			
			//WriteReg ( 2, Data );
			
			break;
			
		// UV
		case 3:
			//Data = ( pValue [ 0 ] & 0x3fff ) | ( ( pValue [ 0 ] >> 16 ) & 0x3fff0000 );
			Data = ( ull0 & 0x3fff ) | ( ( ull0 >> 16 ) & 0x3fff0000 );
			
			//WriteReg ( 3, Data );
			break;
			
		// XYZF2
		case 4:
			//Data = ( pValue [ 0 ] & 0xffff ) | ( ( pValue [ 0 ] >> 16 ) & 0xffff0000 ) | ( ( ( pValue [ 1 ] >> 4 ) & 0xffffff ) << 32 ) | ( ( ( pValue [ 1 ] >> 36 ) & 0xff ) << 56 );
			Data = ( ull0 & 0xffff ) | ( ( ull0 >> 16 ) & 0xffff0000 ) | ( ( ( ull1 >> 4 ) & 0xffffff ) << 32 ) | ( ( ( ull1 >> 36 ) & 0xff ) << 56 );
			
			if ( ( ull1 >> 47 ) & 1 )
			{
				lIndex += 8;
				//WriteReg ( 0xc, Data );
			}
			else
			{
				//WriteReg ( 0x4, Data );
			}
			
			break;
			
		// XYZ2
		case 5:
			//Data = ( pValue [ 0 ] & 0xffff ) | ( ( pValue [ 0 ] >> 16 ) & 0xffff0000 ) | ( ( ( pValue [ 1 ] ) & 0xffffffff ) << 32 );
			Data = ( ull0 & 0xffff ) | ( ( ull0 >> 16 ) & 0xffff0000 ) | ( ( ( ull1 ) & 0xffffffff ) << 32 );
			
			if ( ( ull1 >> 47 ) & 1 )
			{
				lIndex += 8;
				//WriteReg ( 0xd, Data );
			}
			else
			{
				//WriteReg ( 0x5, Data );
			}
			
			break;
			
		// FOG (F)
		case 0xa:
			//Data = ( ( ( pValue [ 1 ] >> 36 ) & 0xff ) << 56 );
			Data = ( ( ( ull1 >> 36 ) & 0xff ) << 56 );
			
			//WriteReg ( 0xa, Data );
			break;
			
		case 0xe:
			//Data = pValue [ 0 ];
			Data = ull0;
			
			//WriteReg ( ull1 & 0xff, ull0 );
			lIndex = ull1 & 0xff;
			break;
			
		// NOP ??
		case 0xf:
			return;
			break;
			
		// write ull0 to the register specified
		default:
			/*
#ifdef INLINE_DEBUG_INVALID
	debug << dec << "\r\nWriteReg_Packed index outside range. lIndex=" << lIndex << " Cycle#" << *_DebugCycleCount;
#endif

			cout << "\nhps2x64: ALERT: WriteReg_Packed encountered invalid REG number.";
			*/
			
			Data = ull0;
			break;
	}

/*
#ifdef INLINE_DEBUG_FIFO
			if ( lIndex < c_iGPURegsGp_Count )
			{
			debug << "; " << GPURegsGp_Names [ lIndex ];
			debug << " (IDX=" << dec << lIndex << " VAL=" << hex << Data << ")";
			}
#endif
*/

	// write the value to register
	WriteReg ( lIndex, Data, ulThreadNum );
}


void GPU::WriteReg ( u32 lIndex, u64 Value, u32 ulThreadNum )
{
	u32 lContext;
	TEX0_t *TEX02;
	TEX0_t *TEX00;

#ifdef ENABLE_MIPMAP
	TEX1_t *TEX1;
	MIPTBP1_t *MTB;
#endif
	
	//u64 *inputdata_ptr;
	u64 NumPixels;
	u64 drawvars_ptr [ 16 ];
	
	// make sure index is within range
	if ( lIndex >= c_iGPURegsGp_Count )
	{
#ifdef INLINE_DEBUG_INVALID
	debug << dec << "\r\nWriteReg index outside range. lIndex=" << lIndex << " Cycle#" << *_DebugCycleCount;
#endif

#ifdef COUT_INDEX_RANGE
		cout << "\nhps2x64 ALERT: GPU: WriteReg index outside range.\n";
#endif

		return;
	}
	
#ifdef INLINE_DEBUG_FIFO
			if ( lIndex < c_iGPURegsGp_Count )
			{
			debug << "; " << GPURegsGp_Names [ lIndex ];
			debug << " (IDX=" << dec << lIndex << " VAL=" << hex << Value << ")";
			}
#endif

	// check if multi-threading gpu
	if ( GPU::ulThreadedGPU )
	{
		// check for trxdir if on cpu thread
		if ( !ulThreadNum )
		{
			// check if reg index is trxdir
			switch ( lIndex )
			{
				case TRXDIR:
					// check for transfer gpu->mem
					if ( ( Value & 3 ) == 1 )
					{
						// update dma
						Dma::_DMA->CheckTransfer ();
					}
					
					// don't do anything else on the cpu thread if multi-threading gpu
					return;
					
					break;
					
				default:
					break;
			}
		}
	}

	// can set register value if main thread is making call
	// otherwise, just set related variables etc
	//if ( !ulThreadNum )
	//{
		// with new code, should set the register only on the correct thread, but should only get called from the correct thread
		GPURegsGp.Regs [ lIndex ] = Value;
	//}
	
	// certain registers require an action be performed
	switch ( lIndex )
	{
		// graphics drawing //
		
		case PRIM:
			// sets the current primitive
			
			// if PRIM register gets written to, looks like it starts the new primitive
			// so clear count of vertexes ??
			lVertexCount = 0;
			break;
		
		case XYZ2:
			// sets value with vertex kick and possible drawing kick
			
			// x -> bits 0-15 -> 12.4 fixed point
			// y -> bits 16-31 -> 12.4 fixed point
			// z -> bits 32-63
			//SetXYZ ( Value );
			
			// vertex kick
			VertexKick ();
			
			// drawing kick depends on the primitive -> I'll let drawing kick handle that...
			DrawingKick ();
			break;
			
		case XYZF2:
			// sets value with vertex kick and possible drawing kick
			
			// x -> bits 0-15 -> 12.4 fixed point
			// y -> bits 16-31 -> 12.4 fixed point
			// z -> bits 32-55
			// f -> bits 56-63
			//SetXYZ ( Value );
			//SetF ( Value );
			
			// also write to XYZ2 (will use XYZ2 as master XYZ register, since multiple registers write to the same place)
#ifdef ENABLE_XYZ_SHIFT
			// only problem is that it is also a good idea to shift the 24-bit z-value so that it is a 32-bit z-value
			GPURegsGp.XYZ2.Value = ( Value & 0xffffffffULL ) | ( ( Value << 8 ) & 0xffffff0000000000ULL );
#else
			// get rid of the F here (top 8-bits)
			GPURegsGp.XYZ2.Value = ( Value << 8 ) >> 8;
#endif
			
			// also write the value to the F register
			GPURegsGp.FOG = Value;
			
			// vertex kick
			VertexKick ();
			
			// drawing kick depends on the primitive -> I'll let drawing kick handle that...
			DrawingKick ();
			break;
			
		case XYZ3:
			// sets value with vertex kick, but no drawing kick
			
			// x -> bits 0-15 -> 12.4 fixed point
			// y -> bits 16-31 -> 12.4 fixed point
			// z -> bits 32-63
			//SetXYZ ( Value );
			
			// also write to XYZ2
			GPURegsGp.XYZ2.Value = Value;
			
			// vertex kick
			VertexKick ();
			
			// still need to reset vertex count for some primitives
			ResetPrimitive ();
			
			break;
			
		case XYZF3:
			// sets value with vertex kick, but no drawing kick
			
			// x -> bits 0-15 -> 12.4 fixed point
			// y -> bits 16-31 -> 12.4 fixed point
			// z -> bits 32-55
			// f -> bits 56-63
			//SetXYZ ( Value );
			//SetF ( Value );
			
			// also write to XYZ2
#ifdef ENABLE_XYZ_SHIFT
			// only problem is that it is also a good idea to shift the 24-bit z-value so that it is a 32-bit z-value
			GPURegsGp.XYZ2.Value = ( Value & 0xffffffffULL ) | ( ( Value << 8 ) & 0xffffff0000000000ULL );
#else
			// get rid of the F here (top 8-bits)
			GPURegsGp.XYZ2.Value = ( Value << 8 ) >> 8;
#endif
			
			// also write the value to the F register
			GPURegsGp.FOG = Value;
			
			// vertex kick
			VertexKick ();
			
			// still need to reset vertex count for some primitives
			ResetPrimitive ();
			
			break;
			
		
		
			
		case TRXDIR:
#ifdef INLINE_DEBUG_TRXDIR
				debug << " (TRXDIR=" << hex << Value << ")";
#endif

		
			// set transfer flags?
			if ( ( Value & 3 ) == 1 )
			{
#ifdef INLINE_DEBUG_TRXDIR
				debug << "\r\nhps2x64: GPU: ALERT: Transfer direction set to gpu->mem.\r\n";
#endif
#ifdef VERBOSE_DEBUG_TRXDIR
				cout << "\nhps2x64: GPU: ALERT: Transfer direction set to gpu->mem.\n";
#endif
				
				// if path 3 is not currently transferring, then set the direction of the bus now
				if ( GIFRegs.STAT.APATH != 3 )
				{
					// transfer direction is gpu->mem
					// directon must also be set in BUSDIR register
					//GIFRegs.STAT.DIR = 1;
				}
				
				// don't want this to happen from gpu thread
				if ( !ulThreadNum )
				{
					// jump start dma transfer
					Dma::_DMA->CheckTransfer ();
				}
			}
			else
			{
				// assume transfer direction is mem->gpu
				//GIFRegs.STAT.DIR = 0;
			}
			
			/*
			if ( ulNumberOfThreads )
			{
				if ( !ulThreadNum )
				{
					return;
				}
			}
			*/

			if ( ( Value & 3 ) == 2 )
			{
				// local->local transfer //
				
#ifdef VERBOSE_LOCAL_TRANSFER
				cout << "\nhps2x64: ALERT: GPU: Local->Local transfer started!!!\n";
#endif

#ifdef USE_TEMPLATES_PS2_COPYLOCAL


				//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );

				//NumPixels = Select_CopyLocal_t ( inputdata_ptr, 0 );
				NumPixels = Select_CopyLocal_t ( drawvars_ptr, 1 );
				

				//if ( BusyUntil_Cycle < *_DebugCycleCount )
				//{
				//	BusyUntil_Cycle = *_DebugCycleCount + ( NumPixels >> 4 );
				//}

				// done
				break;
#else

				// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
				//Finish ();

#endif

			}	// end if ( ( Value & 3 ) == 2 )
#ifdef USE_TEMPLATES_PS2_TRANSFERIN
			else
			{
				//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );

				Select_Start_Transfer_t ( drawvars_ptr, 1, Value );
				
				
				break;
				
			}	// end else if ( !( Value & 3 ) )
#endif

			// only reload all the variables from the register values here
			
			// BITBLTBUF //
			
			// get source buffer offset (SBP : word address/64)
			XferSrcOffset32 = GPURegsGp.BITBLTBUF.SBP << 6;

			if ( GPURegsGp.BITBLTBUF.SBW >= 1 && GPURegsGp.BITBLTBUF.SBW <= 32 )
			{
				// get source buffer width (in pixels) (SBW : number of pixels/64)
				XferSrcBufWidth = GPURegsGp.BITBLTBUF.SBW << 6;
				//XferSrcBufWidth = GPURegsGp.BITBLTBUF.SBW;
			}
			else
			{
#ifdef INLINE_DEBUG_INVALID_SBW
	debug << dec << "\r\nhps2x64: ALERT: Src Buffer Width value illegal. BITBLTBUF.SBW=" << dec << (u32) GPURegsGp.BITBLTBUF.SBW;
#endif

#ifdef VERBOSE_ALERTS_SBW
				cout << "\nhps2x64: ALERT: Src Buffer Width value illegal. BITBLTBUF.SBW=" << dec << (u32) GPURegsGp.BITBLTBUF.SBW;
#endif
			}

			// get dest buffer offset (DBP : word address/64)
			XferDstOffset32 = GPURegsGp.BITBLTBUF.DBP << 6;

			if ( GPURegsGp.BITBLTBUF.DBW >= 1 && GPURegsGp.BITBLTBUF.DBW <= 32 )
			{
				// get dest buffer width (in pixels) (DBW : pixels/64)
				XferDstBufWidth = GPURegsGp.BITBLTBUF.DBW << 6;
				//XferDstBufWidth = GPURegsGp.BITBLTBUF.DBW;
				
				//InternalBufferWidth [ GPURegsGp.BITBLTBUF.DBP >> ( 14 - c_lBufCheckBits ) ] = GPURegsGp.BITBLTBUF.DBW << 6;
			}
			else
			{
#ifdef INLINE_DEBUG_INVALID_DBW
	debug << dec << "\r\nhps2x64: ALERT: Dst Buffer Width value illegal. BITBLTBUF.DBW=" << dec << (u32) GPURegsGp.BITBLTBUF.DBW;
#endif

#ifdef VERBOSE_ALERTS_DBW
if ( GPURegsGp.BITBLTBUF.DBW )
{
				cout << "\nhps2x64: ALERT: Dst Buffer Width value illegal. BITBLTBUF.DBW=" << dec << (u32) GPURegsGp.BITBLTBUF.DBW;
}
#endif
			}

			
			// TRXPOS //
			
			// transfer x,y for source
			XferSrcX = GPURegsGp.TRXPOS.SSAX;
			XferSrcY = GPURegsGp.TRXPOS.SSAY;
			
			
			// transfer x,y for dest
			XferDstX = GPURegsGp.TRXPOS.DSAX;
			XferDstY = GPURegsGp.TRXPOS.DSAY;

			// set current transfer x position
			XferX = 0;
			
			// set current transfer y position
			XferY = 0;
			
			// set pixel shift and count for 24-bit pixels
			PixelShift = 0;
			PixelCount = 0;

			// TRXREG //
			
			// get transfer width (in pixels)
			XferWidth = GPURegsGp.TRXREG.RRW;
			
			// get transfer height (in pixels)
			XferHeight = GPURegsGp.TRXREG.RRH;
			
			
			// ***todo*** get transfer direction, and start transfer immediately if GPU->GPU transfer
			if ( ( Value & 3 ) == 2 )
			{
#ifdef VERBOSE_LOCAL_TRANSFER
				cout << "\nhps2x64: ALERT: GPU: Local->Local transfer started!!!\n";
#endif

				// local transfer //
				
				// alert on an unimplemented transfer method
				//if ( GPURegsGp.TRXPOS.DIR )
				//{
					//cout << "\nhps2x64: ALERT: GPU: unimplemented local transfer method=" << GPURegsGp.TRXPOS.DIR;
					
					// set local->local transfer variables //
					if ( GPURegsGp.TRXPOS.DIR & 2 )
					{
						// transfer is starting on the right //
						xStart = XferWidth - 1;
						xInc = -1;
					}
					else
					{
						// transfer is starting on the left //
						xStart = 0;
						xInc = 1;
					}
					
					if ( GPURegsGp.TRXPOS.DIR & 1 )
					{
						// transfer is starting on the bottom //
						yStart = XferHeight - 1;
						yInc = -1;
					}
					else
					{
						// transfer is starting on the top //
						yStart = 0;
						yInc = 1;
					}
				//}
				
				// set the local transfer variables
				XferX = xStart;
				XferY = yStart;
				

				// perform local->local transfer
				// *** todo *** account for GPU delays and timing
				TransferDataLocal_DS ();

			}	// end if ( ( Value & 3 ) == 2 )
			
			break;

			
		case FINISH:
		
#ifdef VERBOSE_FINISH_WRITE
	cout << "\nhps2x64: GPU: ALERT: writing to FINISH register";
#endif

			// any value written here should trigger finish event if enabled
			
			// check if finish event is enabled in CSR
			if ( !GPURegs1.CSR.FINISH )
			{
				// FINISH Event is enabled
				
				
				// check if should interrupt
				if ( !GPURegs1.IMR.FINISHMSK )
				{
					// finish event has been triggerred and finish interrupt is enabled
					// send interrupt
					SetInterrupt ();

				}

				// set the corresponding bit in CSR
				GPURegs1.CSR.FINISH = 1;

			}
			
			break;

			
		case LABEL:
		
			// any value writen here should update label in SIGBLID register properly
			// but does NOT trigger an interrupt
			
#ifdef VERBOSE_LABEL_WRITE
	cout << "\nhps2x64: GPU: ALERT: writing to LABEL register";
#endif
			
			GPURegs1.SIGLBLID.LBLID = ( ( GPURegs1.SIGLBLID.LBLID ) & ~( Value >> 32 ) ) | ( Value & ( Value >> 32 ) );
			
			break;
			
		
		case SIGNAL:
		
#ifdef VERBOSE_SIGNAL_WRITE
	cout << "\nhps2x64: GPU: ALERT: writing to SIGNAL register";
#endif

			// the signal register sends an interrupt
			// but if there is already an interrupt processing stops until the previous is cleared
			GPURegs1.SIGLBLID.SIGID = ( ( GPURegs1.SIGLBLID.SIGID ) & ~( Value >> 32 ) ) | ( Value & ( Value >> 32 ) );
			
			// check if there is an interrupt already pending
			
			// check if signal interrupt is enabled
			if ( !GPURegs1.IMR.SIGMSK )
			{
				// SIGNAL interrupt is enabled //
				
				// check if signal interrupt is cleared
				if ( !GPURegs1.CSR.SIGNAL )
				{
					// there is no SIGNAL interrupt currently pending //
					
					// trigger interrupt
					SetInterrupt ();

				}
				else
				{
					// SIGNAL interrupt was already pending //

					// *** TODO *** stop gpu processing until SIGNAL interrupt is cleared //
					
					// if signal interrupt is already pending, then gpu processing stops and the new interrupt is queued
					cout << "\nhps2x64: ALERT: SIGNAL interrupt generated when already pending";
				}
				
				// set bit regardless ???
				// set SIGNAL interrupt
				GPURegs1.CSR.SIGNAL = 1;
				
			}
			
			break;
		
		case HWREG:
		
			// can only send to this register when doing mem->gpu transmission
			// otherwise should have no effect
			if ( GPURegsGp.TRXDIR.XDIR )
			{
				// transfer not allowed
				cout << "\nhps2x64: ALERT: GPU: HWREG write when not allowed.\n";
				break;
			}
		
#ifdef VERBOSE_HWREG_WRITE
			cout << "\nhps2x64: GPU: Unimplemented Reg Write: " << GPURegsGp_Names [ lIndex ];
#endif

#ifdef USE_TEMPLATES_PS2_TRANSFERIN
			
			//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );

			//Select_TransferIn_t ( inputdata_ptr, 0, &Value, 1 );
			//Select_TransferIn_t ( drawvars_ptr, 1, &Value, 1 );
			Select_TransferIn_t ( NULL, 1, &Value, 1 );
			

#else
			// writing to hwreg transfers data into GPU memory ??
			TransferDataIn32_DS ( (u32*) ( &Value ), 2 );

#endif

			break;





		// will need this stuff that is commented out since internal registers are set when registers are accessed
		// so will need it to determine when mipmapping is enabled
		/*
		case TEX1_1:
		case TEX1_2:
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			
			break;
			
			
		case MIPTBP1_1:
		case MIPTBP1_2:
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			// here will need to overwrite texture offset and texture buffer size with level zero size for now
			
			break;
			
			
		case MIPTBP2_1:
		case MIPTBP2_2:
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			
			break;
		*/
		
		
		/*
		case XYOFFSET_1:
		case XYOFFSET_2:
		
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			Refresh_XYOFFSET ( lContext, Value );
			
			break;


		case SCISSOR_1:
		case SCISSOR_2:
		
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			Refresh_SCISSOR ( lContext, Value );
			
			break;
		*/


		
		case TEX0_1:
		case TEX0_2:
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
//if ( ( ( Value >> 20 ) & 0x3f ) == 3 )
//{
//	cout << "\nTEX0PSM SET TO 3";
//	debug << "\r\nTEX0PSM SET TO 3: VALUE=" << hex << Value;
//}
			
				Refresh_TEX0 ( lContext, Value );
				
			
			TEX00 = &GPURegsGp.TEX0_1;
			
			// set the master tex02 register
			TEXX [ lContext ].Value = TEX00 [ lContext ].Value;

#ifdef ENABLE_MIPMAP
			// if TEX1.MTBA is set to 1, then set MIPTBP1 automatically
			TEX1 = &GPURegsGp.TEX1_1;
			MTB = &GPURegsGp.MIPTBP1_1;
			if ( TEX1 [ lContext ].MTBA )
			{
				if ( TEX00 [ lContext ].TW == TEX00[ lContext ].TH )
				{
					if ( ( TEX00 [ lContext ].TW >= 5 ) && ( TEX00 [ lContext ].TW <= 10 ) )
					{
						if ( TEX00 [ lContext ].PSM < 0x30 )
						{
							if (
									( TEX00 [ lContext ].PSM == 0x13 )
								|| ( TEX00 [ lContext ].PSM == 0x14 )
								|| ( ( ( TEX00 [ lContext ].PSM < 3 ) || ( TEX00 [ lContext ].PSM > 0x14 ) ) && ( TEX00 [ lContext ].TW < 10 ) )
							)
							{
#ifdef VERBOSE_MIPMAP_BASEPTR
								cout << "\nhps2x64: GPU: Automatic Base Pointer calculation requested.\n";
#endif

								MTB [ lContext ].TBP1 = TEX00 [ lContext ].TBP0 + ( 1 << ( TEX00 [ lContext ].TW + TEX00 [ lContext ].TH ) );
								MTB [ lContext ].TBW1 = TEX00 [ lContext ].TBW0 >> 1;
								MTB [ lContext ].TBP2 = MTB [ lContext ].TBP1 + ( 1 << ( ( TEX00 [ lContext ].TW + TEX00 [ lContext ].TH ) >> 2 ) );
								MTB [ lContext ].TBW2 = TEX00 [ lContext ].TBW0 >> 2;
								MTB [ lContext ].TBP3 = MTB [ lContext ].TBP2 + ( 1 << ( ( TEX00 [ lContext ].TW + TEX00 [ lContext ].TH ) >> 4 ) );
								MTB [ lContext ].TBW3 = TEX00 [ lContext ].TBW0 >> 3;
							}
						}
					}

				}
			}
#endif


#ifdef USE_TEMPLATES_PS2_WRITECLUT

			//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
			
			//Select_WriteInternalCLUT_t( inputdata_ptr, 0, lContext );
			Select_WriteInternalCLUT_t( drawvars_ptr, 0, lContext );
			
#else
			WriteInternalCLUT ( TEX00 [ lContext ] );
			//WriteInternalCLUT ( TEXX [ lContext ] );
#endif

			break;

		case TEX2_1:
		case TEX2_2:
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;

//if ( ( ( Value >> 20 ) & 0x3f ) == 3 )
//{
//	cout << "\nTEX2PSM SET TO 3";
//	debug << "\r\nTEX2PSM SET TO 3: VALUE=" << hex << Value;
//}

			TEX00 = &GPURegsGp.TEX0_1;
			TEX02 = &GPURegsGp.TEX2_1;
			
			TEXX [ lContext ].Value = ( TEX02 [ lContext ].Value & 0xffffffe003f00000ull ) | ( TEXX [ lContext ].Value & ~0xffffffe003f00000ull );
			TEX00 [ lContext ].Value = TEXX [ lContext ].Value;

			
			
			// if single threaded, then refresh tex0 here
			//if( !_GPU->ulNumberOfThreads )
			//{
				// still need to refresh tex0 in case it is single threaded
				Refresh_TEX0 ( lContext, TEX00 [ lContext ].Value );
			//}
			
#ifdef USE_TEMPLATES_PS2_WRITECLUT

			//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
			
			//Select_WriteInternalCLUT_t( inputdata_ptr, 0, lContext );
			Select_WriteInternalCLUT_t( drawvars_ptr, 0, lContext );
			
#else
			// ***TODO*** send palette to internal CLUT
			// TEX2 looks like it is ONLY for changing CLUT info
			// so this will use the clut code used for the TEX0 register
			WriteInternalCLUT ( TEX02 [ lContext ] );
			//WriteInternalCLUT ( TEXX [ lContext ] );
#endif
			
			break;
		
			
		
		case CLAMP_1:
		case CLAMP_2:
		
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
		
			Refresh_CLAMP ( lContext, Value );
		
			break;
		
			
			
			
		case ALPHA_1:
		case ALPHA_2:
		
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			Refresh_ALPHA ( lContext, Value );
			
			break;


		
		case TEST_1:
		case TEST_2:
		
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;


			Refresh_TEST ( lContext, Value );
			
			break;
			
		
		case FBA_1:
		case FBA_2:
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			Refresh_FBA ( lContext, Value );
			
			break;
		
			
		case FRAME_1:
		case FRAME_2:
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			Refresh_FRAME ( lContext, Value );
			
			break;
		
			
			
		case ZBUF_1:
		case ZBUF_2:
		
			
			// internal registers/variables get set when register is accessed
			lContext = lIndex & 1;
			
			Refresh_ZBUF ( lContext, Value );
			
			break;
		
		case COLCLAMP:
		
			
			Refresh_COLCLAMP ( 0, Value );
			break;
		case PABE:
		
			
			Refresh_PABE ( 0, Value );
			break;
		case DTHE:
		
			
			Refresh_DTHE ( 0, Value );
			break;
		case DIMX:
		
			
			Refresh_DIMX ( 0, Value );
			break;
		case FOGCOL:
		
			
			Refresh_FOGCOL ( 0, Value );
			break;
		case TEXCLUT:
		
			
			Refresh_TEXCLUT ( 0, Value );
			break;
		case TEXA:
		
			
			Refresh_TEXA ( 0, Value );
			break;
			
			
			
		/*
		case TEXCLUT:
		
			// color lookup table width is stored in TEXCLUT register (in pixels/64)
			// get clut width and x in pixels
			iclut_width = GPURegsGp.TEXCLUT.CBW << 6;
			iclut_x = GPURegsGp.TEXCLUT.COU << 6;
			
			// get clut y in units of pixels
			iclut_y = GPURegsGp.TEXCLUT.COV;
			
			break;
		*/
			
			
		default:
#ifdef VERBOSE_INVALID_REG_WRITE
			cout << "\nhps2x64: GPU: Unimplemented Reg Write: " << GPURegsGp_Names [ lIndex ];
#endif

			break;
	}
}


void GPU::Start_Frame ( void )
{
	int i;
	int iRet;
	u32 ulThreadCount;
	ulThreadCount = ulNumberOfThreads;

//cout << "\nSTART FRAME";

	
	ulNumberOfThreads_Created = 0;
	
//cout << "\nulThreadCount= " << dec << ulThreadCount;

	//if ( ulThreadCount )
	if ( ulThreadedGPU )
	{
		//cout << "\nGPU::Start_Frame";

		// clear static variables
		ulInputBuffer_WriteIndex = 0;
		ulInputBuffer_TargetIndex = 0;
		ulInputBuffer_TargetIndex2 = 0;
		ulInputBuffer_ReadIndex = 0;

		ullP1Buf_WriteIndex = 0;
		ullP1Buf_TargetIndex = 0;
		ullP1Buf_TargetIndex2 = 0;
		ullP1Buf_ReadIndex = 0;
		
		// create event for triggering gpu thread UPDATE
		ghEvent_PS2GPU_Update = CreateEvent(
			NULL,			// default security
			false,			// auto-reset event (true for manually reset event)
			false,			// initially not set
			NULL			// name of event object
		);
		
		if ( !ghEvent_PS2GPU_Update )
		{
			cout << "\nERROR: Unable to create PS2 GPU UPDATE event. " << GetLastError ();
		}
		
		// create event for triggering/allowing next FRAME
		ghEvent_PS2GPU_Frame = CreateEvent(
			NULL,
			false,
			true,
			NULL
		);
		
		if ( !ghEvent_PS2GPU_Frame )
		{
			cout << "\nERROR: Unable to create PS2 GPU FRAME event. " << GetLastError ();
		}
		
		// create event to trigger FINISH
		ghEvent_PS2GPU_Finish = CreateEvent(
			NULL,
			true,
			false,
			NULL
		);
		
		if ( !ghEvent_PS2GPU_Finish )
		{
			cout << "\nERROR: Unable to create PS2 GPU FINISH event. " << GetLastError ();
		}
		
		// ***todo*** reset write index
		//ulInputBuffer_WriteIndex = 0;
		
		// clear read index
		//ulInputBuffer_ReadIndex = 0;
		
		// transfer to target index
		Lock_Exchange64 ( (long long&) ulInputBuffer_TargetIndex, ulInputBuffer_WriteIndex );

		if ( ulThreadCount )
		{
			//for ( int i = 0; i < ulNumberOfThreads; i++ )
			for ( i = 0; i < ulThreadCount; i++ )
			{
				//cout << "\nCreating GPU thread#" << dec << i;
				
				// create thread
				GPUThreads [ i ] = new Api::Thread( Start_GPUThread, (void*) inputdata, false );
				
				//cout << "\nCreated GPU thread#" << dec << i << " ThreadStarted=" << GPUThreads[ i ]->ThreadStarted;
				
				// reset index into buffer
				ullInputBuffer_Index = 0;
				
				ulNumberOfThreads_Created++;
				
//cout << "\nCREATING: NEW GPU THREAD: " << dec << ulNumberOfThreads_Created;
			}

			// wait for thread to be ready
			WaitForSingleObject ( ghEvent_PS2GPU_Finish, INFINITE );

		}
	}
}

void GPU::End_Frame ( void )
{
	int i;
	int iRet;
	u64 *inputdata_ptr;
	
//cout << "\nEND FRAME";
	
	if ( ulNumberOfThreads_Created )
	{
		
		
		//cout << "\nGPU::End_Frame";
		
		//while ( Playstation1::SPU2::ulDeviceLock );
		//while( Lock_ExchangeAdd32( (long&) Playstation1::SPU2::ulDeviceLock, 0 ) );
		
		//if ( ulNumberOfThreads )
		//{
#ifdef USE_NEW_INPUTFORMAT
			inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = *_DebugCycleCount;
			ulInputBuffer_WriteIndex++;
			inputdata [ ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ] = 5;
			ulInputBuffer_WriteIndex++;
#else
			inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
			
			// send command to kill the threads
			Select_KillGpuThreads_t ( inputdata_ptr, 0 );
#endif
			
			Flush ();
		//}
		

		//for ( int i = 0; i < ulNumberOfThreads; i++ )
		for ( i = 0; i < ulNumberOfThreads_Created; i++ )
		{
			//cout << "\nKilling GPU thread#" << dec << i << " ThreadStarted=" << GPUThreads[ i ]->ThreadStarted;
			
			// create thread
			iRet = GPUThreads [ i ]->Join();
			
			//cout << "\nThreadStarted=" << GPUThreads[ i ]->ThreadStarted;
			
			if ( iRet )
			{
				cout << "\nhps1x64: GPU: ALERT: Problem with completion of GPU thread#" << dec << i << " iRet=" << iRet;
			}
			
			delete GPUThreads [ i ];
			
			//cout << "\nKilled GPU thread#" << dec << i << " iRet=" << iRet;
		}
		
		
		ulNumberOfThreads_Created = 0;
		
	}	// if ( ulNumberOfThreads_Created )
	
	if ( ulThreadedGPU )
	{
		// remove the events for now
		CloseHandle ( ghEvent_PS2GPU_Finish );
		CloseHandle ( ghEvent_PS2GPU_Frame );
		CloseHandle ( ghEvent_PS2GPU_Update );

	}	// if ( ulThreadedGPU )

}


int GPU::Start_GPUThread( void* Param )
{
	//u64 ullStartTimer = 0, ullEndTimer = 0;

	u64 ulTBufferTarget;
	u64 ulP1BufferTarget;

	u64 *circularlist;

	circularlist = (u64*) _GPU->inputdata;


	ulTBufferIndex = 0;
	ulP1BufferIndex = 0;

//cout << "\nSTARTING: GPU THREAD";

	// infinite loop
	while ( 1 )
	{
		
		// wait for data to appear in the input buffers
		//while ( ( ulTBufferIndex == ulInputBuffer_TargetIndex ) && ( !Lock_ExchangeAdd32( (long&) Playstation1::SPU2::ulDeviceLock, 0 ) ) );
		//while ( ulTBufferIndex == ulInputBuffer_TargetIndex );
		//while ( ulTBufferIndex == Lock_ExchangeAdd64( (long long&) ulInputBuffer_WriteIndex, 0 ) )
		
	//cout << "\nWaiting for next UPDATE event";

		//ulTBufferTarget = ulInputBuffer_TargetIndex;
		//ulP1BufferTarget = _GPU->ullP1Buf_TargetIndex;





		//ulTBufferTarget = Lock_ExchangeAdd64( (long long&) ulInputBuffer_TargetIndex, 0 );
		//ulP1BufferTarget = Lock_ExchangeAdd64( (long long&) _GPU->ullP1Buf_TargetIndex, 0 );
		ulTBufferTarget = 0;
		ulP1BufferTarget = 0;

		while (
				ulTBufferIndex >= ulTBufferTarget
			&&	ulP1BufferIndex >= ulP1BufferTarget
		)
		{
#ifdef ENABLE_WAIT_DURING_SPIN_THREAD
			if ( !SetEvent ( ghEvent_PS2GPU_Finish ) )
			{
				cout << "\nUnable to set finish event. " << GetLastError ();
			}
			
			WaitForSingleObject( ghEvent_PS2GPU_Update, INFINITE );
#endif

			ulTBufferTarget = Lock_ExchangeAdd64( (long long&) ulInputBuffer_TargetIndex, 0 );
			ulP1BufferTarget = Lock_ExchangeAdd64( (long long&) _GPU->ullP1Buf_TargetIndex, 0 );

		}


		if ( !ResetEvent ( ghEvent_PS2GPU_Finish ) )
		{
			cout << "\nUnable to reset finish event. " << GetLastError ();
		}


		// get the start timer value for the run
		//if ( !QueryPerformanceCounter ( (LARGE_INTEGER*) &ullStartTime ) )
		//{
		//	cout << "\nhpsx64: PS2GPUThread: Error returned from QueryPerformanceCounter at StartTimer\n";
		//}



	//cout << "\nGPU Thread triggered: t1gputargetidx=" << dec << ulInputBuffer_TargetIndex << " p1gputargetidx=" << _GPU->ullP1Buf_TargetIndex;
	//cout << " ulTBufferIndex=" << ulTBufferIndex << " ulP1BufferIndex=" << ulP1BufferIndex;


		if ( !Process_GPUCommandsMT ( ulInputBuffer_TargetIndex, ullP1Buf_TargetIndex ) )
		{
			cout << "\nGPU THREAD KILLED\n";
			break;
		}


//cout << "\nGPU THREAD COMPLETE: " << dec << " t1readidx=" << ulTBufferIndex << " p1readidx=" << ulP1BufferIndex;
		
		
	}	// end while ( 1 )


	return 0;
}


// process multi-threaded gpu commands
int GPU::Process_GPUCommandsMT ( u64 TargetIdx_InputBuffer, u64 TargetIdx_P1Buffer )
{

	u64 *p_inputdata;
	u64 *p1_inputdata;

	u64 Arg0, Arg1;

	u64 ullTCycle;
	u64 ullP1Cycle;
	u64 ullCount;


	u64 ulCountLeft64;
	u64 ulRunCount1, ulRunCount2;
	
	u32 ulLocalCount;
	u32 ulRunCount;
	u32 ulDestReg;
	u64 ullRegs;
	
	GPU::GIFTag0_t gt0;
	GPU::GIFTag1_t gt1;
		
	u64 ulRegCount;
	u64 ullDataCount;

	u64 ulNumRegs;
	u64 ulTransferSize;

	u64 *circularlist;

	circularlist = (u64*) _GPU->inputdata;


#ifdef ENABLE_THREAD_LOCK
		//while ( ulTBufferIndex != ulInputBuffer_WriteIndex )
	while (
			ulTBufferIndex < TargetIdx_InputBuffer
		||	ulP1BufferIndex < TargetIdx_P1Buffer
	)
	{
#endif

#ifdef USE_NEW_INPUTFORMAT
		p_inputdata = & ( circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ] );
#else
		p_inputdata = & ( circularlist [ ( ulTBufferIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
#endif


		p1_inputdata = (u64*) & ( _GPU->ullPath1Buffer [ ( ulP1BufferIndex & c_iPath1BufMask ) << 1 ] );


		// get cycle# for path 1 data
		ullP1Cycle = -1ull;
		if ( ulP1BufferIndex < TargetIdx_P1Buffer )
		{
			ullP1Cycle = p1_inputdata [ 0 ];
		}

		// get cycle# for path 2/3 data
		ullTCycle = -1ull;
		if ( ulTBufferIndex < TargetIdx_InputBuffer )
		{
#ifdef USE_NEW_INPUTFORMAT
			ullTCycle = p_inputdata [ 0 ];
			p_inputdata = & ( circularlist [ ( ulTBufferIndex + 1 ) & c_ulInputBuffer_Mask ] );
			// if this command can't be interrupted by path1, then put path1 on hold for now
			if ( p_inputdata [ 0 ] & 0x10 )
			{
				ullP1Cycle = -1ull;
			}
#else
			ullTCycle = p_inputdata [ 14 ];

			// if this command can't be interrupted by path1, then put path1 on hold for now
			if ( p_inputdata [ 15 ] & 0x10 )
			{
				ullP1Cycle = -1ull;
			}
#endif
		}

//cout << "\nTCycle=" << dec << ullTCycle << " PCycle=" << ullP1Cycle;
//cout << "\nTTargetIdx=" << dec << ulInputBuffer_TargetIndex << " PTargetIdx=" << _GPU->ullP1Buf_TargetIndex;

		// path 1 has priority
		if ( ullP1Cycle <= ullTCycle )
		{
			// path 1 data format (128-bit data)

	//cout << "\nProcessing path1\n";

			// get the amount of data
			ullCount = p1_inputdata [ 1 ];
			ulP1BufferIndex++;

			//cout << "\nP1Cycle=" << ullP1Cycle << " TCycle=" << ullTCycle << " ullCount=" << ullCount << " Target=" << _GPU->ullP1Buf_TargetIndex;
			//cout << "\np1readidx=" << ulP1BufferIndex << " ullCount=" << ullCount << " Target=" << _GPU->ullP1Buf_TargetIndex;

			while ( ullCount )
			{
				p1_inputdata = (u64*) & ( _GPU->ullPath1Buffer [ ( ulP1BufferIndex & c_iPath1BufMask ) << 1 ] );

				// get gif tags
				gt0.Value = *p1_inputdata++;
				gt1.Value = *p1_inputdata++;
				ulP1BufferIndex++;
				ullCount--;


				if ( gt0.PRE && ( gt0.FLG < 2 ) )
				{
#ifdef INLINE_DEBUG_P1FIFO
					debug << "; PRE";
					debug << " PRIM=" << hex << gt0.PRIM;
#endif

					_GPU->WriteReg ( PRIM, gt0.PRIM, 1 );
				}

				ulNumRegs = ( ( !gt0.REGS ) ? ( 16 ) : ( gt0.REGS ) );
				ulTransferSize = gt0.NLOOP * ulNumRegs;

				//cout << "\nNumRegs=" << ulNumRegs << " TransferSize=" << ulTransferSize;

				// loop through the data
				ulRegCount = 0;
				if ( ulTransferSize && ullCount )
				{
					if ( gt0.FLG == 1 )
					{
						for ( int i = 0; i < ulTransferSize; i+=2 )
						{
							p1_inputdata = (u64*) & ( _GPU->ullPath1Buffer [ ( ulP1BufferIndex & c_iPath1BufMask ) << 1 ] );

							// get the register to send to
							ulDestReg = ( gt1.Value >> ( ulRegCount << 2 ) ) & 0xf;
							
							// update reg count
							ulRegCount++;
							
							// if greater or equal to number of registers, reset
							if ( ulRegCount >= ulNumRegs ) ulRegCount = 0;

							// get reg data
							Arg0 = *p1_inputdata++;
							Arg1 = *p1_inputdata++;
							ulP1BufferIndex++;
							ullCount--;

							// 128-bit command data //
							_GPU->WriteReg ( ulDestReg, Arg0, 1 );

							if ( ( ulTransferSize - i ) > 1 )
							{
								// get the register to send to
								ulDestReg = ( gt1.Value >> ( ulRegCount << 2 ) ) & 0xf;
								
								// update reg count
								ulRegCount++;

								// 128-bit command data //
								_GPU->WriteReg ( ulDestReg, Arg1, 1 );
							}

							if ( !ullCount ) break;

						}	// end for ( int i = 0; i < ulTransferSize; i++ )

					}
					else
					{
						for ( int i = 0; i < ulTransferSize; i++ )
						{
							p1_inputdata = (u64*) & ( _GPU->ullPath1Buffer [ ( ulP1BufferIndex & c_iPath1BufMask ) << 1 ] );

							// get the register to send to
							ulDestReg = ( gt1.Value >> ( ulRegCount << 2 ) ) & 0xf;
							
							// update reg count
							ulRegCount++;
							
							// if greater or equal to number of registers, reset
							if ( ulRegCount >= ulNumRegs ) ulRegCount = 0;

							// get reg data
							Arg0 = *p1_inputdata++;
							Arg1 = *p1_inputdata++;
							ulP1BufferIndex++;
							ullCount--;

							if ( ! gt0.FLG )
							{
								// 128-bit command data //
								_GPU->WriteReg_Packed ( ulDestReg, Arg0, Arg1, 1 );
							}
							else
							{
								// 128-bit image data or disabled //
								_GPU->TransferDataIn32_DS ( (u32*) &Arg0, 2 );							
								_GPU->TransferDataIn32_DS ( (u32*) &Arg1, 2 );							
							}

							if ( !ullCount ) break;

						}	// end for ( int i = 0; i < ulTransferSize; i++ )

					}

				}	// end if ( ulTransferSize )

				//cout << "\nullCount=" << ullCount << " P1Idx=" << ulP1BufferIndex;

			}	// end while ( ullCount )

			//cout << "\np1done";

		}
		else
		{
		
//debug << "\nulInputBuffer_Count=" << dec << _GPU->ulInputBuffer_Count;
//debug << "\nullInputBuffer_Index=" << dec << _GPU->ullInputBuffer_Index;
//debug << "\nulTBufferIndex=" << dec << ulTBufferIndex;
		
//cout << "\nPROCESSING: " << dec << ulInputBuffer_ReadIndex << " IDX=" << ( p_inputdata [ 15 ] & 0xf ) << " IDX2=" << ( p_inputdata [ 14 ] );

			//cout << "\nProcessing path2/3 readidx=" << ulTBufferIndex << " targetidx=" << TargetIdx_InputBuffer << " data=" << hex << p_inputdata [ 0 ] << " cycle#" << dec << ullTCycle;

		// get the command
#ifdef USE_NEW_INPUTFORMAT
		switch ( p_inputdata [ 0 ] & 0xf )
#else
		switch ( p_inputdata [ 15 ] & 0xf )
#endif
		{
			case 0:
				// frame has been drawn now, or will be drawn
				//x64ThreadSafe::Utilities::Lock_Exchange64 ( (long long&) ullFrameDrawPending, 0 );
				//MsgWaitForMultipleObjectsEx( NULL, NULL, 1, QS_ALLINPUT, MWMO_ALERTABLE );


#ifdef USE_TEMPLATES_PS2_DRAWSCREEN

#ifdef USE_NEW_INPUTFORMAT
				ulTBufferIndex += 2;
				Select_DrawScreen_t ( (u64*) display_inputbuffer, 1 );
#else
				Select_DrawScreen_t ( p_inputdata, 1 );
#endif


#endif

				// get the end timer value for the run
				if ( !QueryPerformanceCounter ( (LARGE_INTEGER*) &ullEndTime ) )
				{
					cout << "\nhpsx64: PS2GPUThread: Error returned from QueryPerformanceCounter at EndTimer\n";
				}


				// shouldn't trigger next frame event until after screen is drawn
				SetEvent( ghEvent_PS2GPU_Frame );

				break;
						
						

			case 4:
#ifdef USE_TEMPLATES_PS2_TRANSFERIN
				// copy data into GPU
				Select_TransferIn_t ( p_inputdata, 1 );
#endif
				break;

			case 5:
				// Kill GPU Thread
#ifdef USE_NEW_INPUTFORMAT
				ulTBufferIndex += 2;
#else
				ulTBufferIndex++;
#endif

				Lock_Exchange64 ( (long long&) ulInputBuffer_ReadIndex, ulTBufferIndex );
						
//cout << "\nENDING: GPU THREAD: " << dec << ulInputBuffer_ReadIndex << " IDX=" << ( p_inputdata [ 15 ] & 0xf ) << " IDX2=" << ( p_inputdata [ 14 ] );
				return 0;
				break;
			
				
			// 128-bit register values
			case 8:
//cout << "\n128-bit DATA";

				// check if flag is set to reset vertex count
#ifdef USE_NEW_INPUTFORMAT
				if ( ((u16*)p_inputdata) [ 3 ] )
				{
					_GPU->lVertexCount = 0;
				}

				// loop through the registers in packet
				ulRunCount = ((u16*)p_inputdata) [ 1 ];
				ulTBufferIndex += 2;
				ullRegs = circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ];
				ulTBufferIndex++;
#else
				if ( ((u16*)p_inputdata) [ 63 ] )
				{
					_GPU->lVertexCount = 0;
				}
				
				// loop through the registers in packet
				ulRunCount = ((u16*)p_inputdata) [ 61 ];
				ullRegs = p_inputdata [ 13 ];
#endif
				
#ifdef INLINE_DEBUG_FIFO_MT
	debug << " [128-bit REGS]";
	debug << " ulRunCount=" << dec << ulRunCount;
	debug << " ullRegs=" << hex << ullRegs;
	debug << " TIDX=" << dec << ( ulTBufferIndex & c_ulInputBuffer_Mask );
#endif

				for ( int i = 0; i < ulRunCount; i++ )
				{
#ifdef INLINE_DEBUG_FIFO_MT
	debug << " (i=" << dec << i << ")";
	debug << " arg0=" << hex << circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ];
	debug << " arg1=" << hex << circularlist [ (ulTBufferIndex+1) & c_ulInputBuffer_Mask ];
#endif

					ulDestReg = ullRegs & 0xf;
					
					ullRegs >>= 4;
					
					//WriteReg_Packed ( ulDestReg, Arg0, Arg1 );
#ifdef USE_NEW_INPUTFORMAT
					_GPU->WriteReg_Packed ( ulDestReg, circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], circularlist [ (ulTBufferIndex+1) & c_ulInputBuffer_Mask ], 1 );
					ulTBufferIndex += 2;
#else
					_GPU->WriteReg_Packed ( ulDestReg, p_inputdata [ ( i << 1 ) + 0 ], p_inputdata [ ( i << 1 ) + 1 ], 1 );
#endif

				}
				
				
				break;
				
			// 64-bit register values
			case 9:
//cout << "\n64-bit DATA";

#ifdef USE_NEW_INPUTFORMAT
				if ( ((u16*)p_inputdata) [ 3 ] )
				{
					_GPU->lVertexCount = 0;
				}

				// loop through the registers in packet
				ulRunCount = ((u16*)p_inputdata) [ 1 ];
				ulTBufferIndex += 2;
				ullRegs = circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ];
				ulTBufferIndex++;
#else
				// check if flag is set to reset vertex count
				if ( ((u16*)p_inputdata) [ 63 ] )
				{
					_GPU->lVertexCount = 0;
				}
				
				// loop through the registers in packet
				ulRunCount = ((u16*)p_inputdata) [ 61 ];
				ullRegs = p_inputdata [ 13 ];
#endif

#ifdef INLINE_DEBUG_FIFO_MT
	debug << " [64-bit REGS]";
	debug << " ulRunCount=" << dec << ulRunCount;
	debug << " ullRegs=" << hex << ullRegs;
	debug << " TIDX=" << dec << ( ulTBufferIndex & c_ulInputBuffer_Mask );
#endif

				for ( int i = 0; i < ulRunCount; i++ )
				{
#ifdef INLINE_DEBUG_FIFO_MT
	debug << " (i=" << dec << i << ")";
#endif

					ulDestReg = ullRegs & 0xf;
					
					ullRegs >>= 4;
					
#ifdef USE_NEW_INPUTFORMAT
					_GPU->WriteReg ( ulDestReg, circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], 1 );
					ulTBufferIndex++;
#else
					_GPU->WriteReg ( ulDestReg, p_inputdata [ i ], 1 );
#endif
				}
				
				break;
				
			case 10:
//cout << "\nIMAGE DATA";

#ifdef USE_NEW_INPUTFORMAT
				if ( ((u16*)p_inputdata) [ 3 ] )
				{
					_GPU->lVertexCount = 0;
				}

				// loop through the registers in packet
				ulRunCount = ((u16*)p_inputdata) [ 1 ];
				ulTBufferIndex += 2;

				// get the maximum space left in 64-bit units before buffer wraps around
				ulCountLeft64 = c_ulInputBuffer_Size - ( ulTBufferIndex & c_ulInputBuffer_Mask );

				// check if there is enough to transfer in one run or not
				if ( ulCountLeft64 >= ulRunCount )
				{
					// will not encounter a wraparound in buffer //
					ulRunCount1 = ulRunCount;
					ulRunCount2 = 0;
				}
				else
				{
					// buffer will wraparound -> need 2 transfers //
					ulRunCount1 = ulCountLeft64;
					ulRunCount2 = ulRunCount - ulCountLeft64;
				}

#ifdef USE_TEMPLATES_PS2_TRANSFERIN
				_GPU->Select_TransferIn_t ( NULL, 1, & circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], ulRunCount1 );
#else
				_GPU->TransferDataIn32_DS ( (u32*) & circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], ulRunCount1 << 1 );
#endif

				ulTBufferIndex += ulRunCount1;

				if ( ulRunCount2 )
				{
#ifdef USE_TEMPLATES_PS2_TRANSFERIN
					_GPU->Select_TransferIn_t ( NULL, 1, & circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], ulRunCount2 );
#else
					_GPU->TransferDataIn32_DS ( (u32*) & circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], ulRunCount2 << 1 );
#endif

					ulTBufferIndex += ulRunCount2;
				}

				/*
				for ( int i = 0; i < ulRunCount; i++ )
				{
					_GPU->TransferDataIn32_DS ( (u32*) & circularlist [ ulTBufferIndex & c_ulInputBuffer_Mask ], 2 );
					ulTBufferIndex++;
				}
				*/
#else
				// check if flag is set to reset vertex count
				if ( ((u16*)p_inputdata) [ 63 ] )
				{
					_GPU->lVertexCount = 0;
				}
				
				ulRunCount = ((u16*)p_inputdata) [ 61 ];
				_GPU->TransferDataIn32_DS ( (u32*) p_inputdata, ulRunCount << 1 );
#endif
				
				break;
				
			default:
				break;
				
		}	// end switch ( p_inputdata [ 15 ] & 0x7 )
		
#ifndef USE_NEW_INPUTFORMAT
		ulTBufferIndex++;
#endif

		}	// end if ( ullP1Cycle <= ullTCycle ) else

		//cout << "\npath2/3 done";
		
		}	// end while ( ulTBufferIndex != ulInputBuffer_TargetIndex )
			
		//_GPU->ullP1Buf_ReadIndex = ulP1BufferIndex;
		Lock_Exchange64 ( (long long&) _GPU->ullP1Buf_ReadIndex, ulP1BufferIndex );
		Lock_Exchange64 ( (long long&) ulInputBuffer_ReadIndex, ulTBufferIndex );

	return 1;

}





void GPU::WriteInternalCLUT ( TEX0_t TEX02 )
{
	u32 lCLD, CLUTBufBase32, PixelFormat, CLUTPixelFormat, CLUTOffset, CLUTStoreMode;
	u32 clut_width, clut_x, clut_y;
	u32 lPixelCount, lPixelSize;
	
	u32 x, y;
	
	u32 lIndex, lConvertedIndex;
	u32 bgr;
	
	u16 *ptr_clut16;
	u32 *ptr_clut32;
	
	u16 *CLUT_LUT;
	
	u32 ClutBufWidth;

	u32 ulStartPage, ulPageWidth, ulPageHeight;

	u64 *inputdata_ptr;
	
	// check if psm is an indexed color format that requires CLUT
	if ( ( !( TEX02.PSM >> 4 ) ) || ( TEX02.PSM >> 4 ) == 0x3 )
	{
		// this is not an indexed pixel format so has nothing to do with CLUT
		return;
	}
	
	
#ifdef ALLOW_OPENCL_PS2_WRITECLUT
	//bEnable_OpenCL = true;
	if ( bEnable_OpenCL )
	{
		inputdata_ptr = & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];
		inputdata_ptr [ 0 ] = TEX02.Value;
		inputdata_ptr [ 1 ] = GPURegsGp.TEXCLUT.Value;

		// put in the command
		inputdata_ptr [ 15 ] = 0xf0000000;

		// send command
		//Copy_VRAM_toGPU ();


		// dispatch our test command
		//FlushToHardware( 0, 1 );

		// get result
		//Copy_VRAM_toCPU ();

		// submit command
		ulHwInputBuffer_WriteIndex++;


		// done
		return;
	}

#endif


#ifndef USE_TEMPLATES_PS2_CLUT

	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	//Finish ();

#endif



	// if using gpu compute, then run tests
	

	// ***TODO*** send palette to internal CLUT
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	

	// clut offset ??
	CLUTOffset = TEX02.CSA;
	

	// check cld
	switch ( TEX02.CLD )
	{
		case 0:
			// do not load into temp CLUT //
			return;
			break;
			
		case 1:
		//case 7:
			// always load //
			break;
			
		case 2:
		//case 6:
			// load and copy CBP to CBP0 //
			//CBP0 = TEX02.CBP;
			CBPX[0] = TEX02.CBP;
			break;
			
		case 3:
			// load and copy CBP to CBP1 //
			//CBP1 = TEX02.CBP;
			CBPX[1] = TEX02.CBP;
			break;
			
		case 4:
			// load and copy CBP to CBP0 only if CBP<>CBP0 //
			//if ( TEX02.CBP == CBP0 ) return;
			if ( TEX02.CBP == CBPX[0] ) return;
			//CBP0 = TEX02.CBP;
			CBPX[0] = TEX02.CBP;
			break;
			
		case 5:
			// load and copy CBP to CBP1 only if CBP<>CBP1 //
			//if ( TEX02.CBP == CBP1 ) return;
			if ( TEX02.CBP == CBPX[1] ) return;
			//CBP1 = TEX02.CBP;
			CBPX[1] = TEX02.CBP;
			break;
	}
	
	
	// the clut offset is actually CSA times 16 pixels
	CLUTOffset <<= 4;
	
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX02.CBP << 6;
	
	// get pointer into CLUT in local memory
	ptr_clut32 = & ( RAM32 [ CLUTBufBase32 ] );
	ptr_clut16 = (u16*)ptr_clut32;
	
	
	// need to know if texture is 4-bit or 8-bit (determines how many pixels to send into CLUT)
	//PixelFormat = TEX02.PSM;
	//CLUTPixelFormat = TEX02.CPSM;
	//lCLD = TEX02.CLD;
	// storage mode ??
	//CLUTStoreMode = TEX02.CSM;
	

	//ClutBufWidth = GPURegsGp.BITBLTBUF.DBW;
	ClutBufWidth = InternalBufferWidth [ TEX02.CBP >> ( 14 - c_lBufCheckBits ) ];


	
	
	// transfer pixels
	
	// need to know if pixels are 4/8-bit
	if ( ( TEX02.PSM & 7 ) > 2 )
	{
		// will need to write into the internal CLUT
		
		// get the number of pixels to transfer
		if ( TEX02.PSM & 0x4 )
		{
			// 4-bit pixels - 16 colors
			lPixelCount = 16;
			
			/*
			if ( ClutBufWidth == 1 )
			{
				ClutBufWidth = 8;
			}
			else
			{
				ClutBufWidth *= 64;
			}
			*/
		}
		else
		{
			// 8-bit pixels - 256 colors
			lPixelCount = 256;
			
			/*
			if ( ClutBufWidth == 1 )
			{
				ClutBufWidth = 16;
			}
			else
			{
				ClutBufWidth *= 64;
			}
			*/
		}

		
//cout << "\nTEX_CLD=" << TEX02.CLD << " CLUT_CSM=" << hex << TEX02.CSM << " CLUT_PSM=" << TEX02.CPSM << " PIX_COUNT=" << ( TEX02.PSM & 0x4 ) << " c_ulPixelCount=" << lPixelCount << " CLUTOffset=" << CLUTOffset << " CLUTBufBase32=" << CLUTBufBase32;
	
		if ( !TEX02.CSM )
		{
			// CSM1 //
			//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
			CLUT_LUT = (u16*) ucCLUT_Lookup_CSM01;
			
			// need to determine size of pixels to transfer into clut
			// need to know if pixels are 16 or 32 bit
			if ( TEX02.CPSM & 0x2 )
			{
				// 16-bit pixels //
				
				for ( lIndex = 0; lIndex < lPixelCount; lIndex++ )
				{
#ifdef ENABLE_DATA_STRUCTURE
					switch ( lPixelCount )
					{
						case 16:
							switch ( TEX02.CPSM )
							{
								case 2:
									bgr = ptr_clut16 [ CvtAddrPix16 ( lIndex & 7, lIndex >> 3, 0 ) ];
									break;
									
								case 0xa:
									bgr = ptr_clut16 [ CvtAddrPix16S ( lIndex & 7, lIndex >> 3, 0 ) ];
									break;
							}
								
							break;
							
						case 256:
							switch ( TEX02.CPSM )
							{
								case 2:
									bgr = ptr_clut16 [ CvtAddrPix16 ( ( lIndex & 0x7 ) | ( ( lIndex & 0x10 ) >> 1 ), ( ( lIndex >> 4 ) & 0xe ) | ( ( lIndex >> 3 ) & 1 ), 0 ) ];
									break;
									
								case 0xa:
									bgr = ptr_clut16 [ CvtAddrPix16S ( ( lIndex & 0x7 ) | ( ( lIndex & 0x10 ) >> 1 ), ( ( lIndex >> 4 ) & 0xe ) | ( ( lIndex >> 3 ) & 1 ), 0 ) ];
									break;
							}
							
							break;
					}
#else
					// lookup another index value ??
					lConvertedIndex = CLUT_LUT [ lIndex ];
					
					lConvertedIndex = ( ( lConvertedIndex >> 4 ) * ClutBufWidth ) + ( lConvertedIndex & 0xf );
					
					// in the case of CSM01, need to swap bits 3 and 4
					bgr = ptr_clut16 [ lConvertedIndex ];
#endif
					
					// 512-entry 16-bit pixels
					InternalCLUT [ ( CLUTOffset + lIndex ) & 511 ] = bgr;
				}

			}
			else
			{
				// 32-bit pixels //
				
				// only 4-bits of Offset are valid
				CLUTOffset &= 255;
				
				// transfer pixels
				for ( lIndex = 0; lIndex < lPixelCount; lIndex++ )
				{
#ifdef ENABLE_DATA_STRUCTURE
					switch ( lPixelCount )
					{
						case 16:
							bgr = ptr_clut32 [ CvtAddrPix32 ( lIndex & 7, lIndex >> 3, 0 ) ];
							break;
							
						case 256:
							bgr = ptr_clut32 [ CvtAddrPix32 ( ( lIndex & 0x7 ) | ( ( lIndex & 0x10 ) >> 1 ), ( ( lIndex >> 4 ) & 0xe ) | ( ( lIndex >> 3 ) & 1 ), 0 ) ];
							break;
					}
#else
					// lookup another index value ??
					lConvertedIndex = CLUT_LUT [ lIndex ];
					
					lConvertedIndex = ( ( lConvertedIndex >> 4 ) * ClutBufWidth ) + ( lConvertedIndex & 0xf );
					
					// in the case of CSM01, need to swap bits 3 and 4
					bgr = ptr_clut32 [ lConvertedIndex ];
#endif

//cout << "\nlIndex#" << dec << lIndex << " bgr=" << hex << bgr;

					// 512-entry 16-bit pixels
					InternalCLUT [ ( CLUTOffset + lIndex ) & 511 ] = bgr;
					
					// the upper part of buffer gets the high bits
					InternalCLUT [ ( CLUTOffset + lIndex + 256 ) & 511 ] = ( bgr >> 16 );
				}
			}
		}
		else
		{
			// CSM2 //
			CLUT_LUT = (u16*) ucCLUT_Lookup_CSM02;
			
			// CBW is in units of pixels/64
			clut_width = GPURegsGp.TEXCLUT.CBW << 6;
			
			// COU is in units of pixels/16
			//clut_x = GPURegsGp.TEXCLUT.COU << 6;
			clut_x = GPURegsGp.TEXCLUT.COU << 4;
			
			// get clut y in units of pixels
			clut_y = GPURegsGp.TEXCLUT.COV;
			
			// in CSM2 mode, the size of the pixels is always 16-bit and can only specify PSMCT16 //
			
#ifndef ENABLE_DATA_STRUCTURE
			// 16-bit pixels //
			// use clut_x, clut_y, clut_width
			ptr_clut16 = & ( ptr_clut16 [ ( clut_x ) + ( clut_y * clut_width ) ] );
#endif
			
			// 16-bit pixels //
			
			for ( lIndex = 0; lIndex < lPixelCount; lIndex++ )
			{
#ifdef ENABLE_DATA_STRUCTURE
				bgr = ptr_clut16 [ CvtAddrPix16 ( clut_x + lIndex, clut_y, clut_width ) ];
#else
				// lookup another index value ??
				lConvertedIndex = CLUT_LUT [ lIndex ];
				
				// in the case of CSM01, need to swap bits 3 and 4
				bgr = ptr_clut16 [ lConvertedIndex ];
#endif
				
				// 512-entry 16-bit pixels
				InternalCLUT [ ( CLUTOffset + lIndex ) & 511 ] = bgr;
			}
		}
		
	}

}



void GPU::VertexKick ()
{
	u32 lIndex;
	
	// get index to write coords into
	lIndex = lVertexQ_Index & c_iVertexQ_Mask;
	
	// need to save the first point in case it is a triangle fan
	// also can possibly be used to get color for mono polygon, etc ??
	// vertex count actually had the count of vertexes drawn
	//if ( !lVertexQ_Index )
	if ( !lVertexCount )
	{
		//xyz [ 4 ].Value = xyz [ lVertexQ_Index & c_iVertexQ_Mask ].Value;
		//uv [ 4 ].Value = uv [ lVertexQ_Index & c_iVertexQ_Mask ].Value;
		//st [ 4 ].Value = st [ lVertexQ_Index & c_iVertexQ_Mask ].Value;
		//rgbaq [ 4 ].Value = rgbaq [ lVertexQ_Index & c_iVertexQ_Mask ].Value;
		//f [ 4 ].Value = f [ lVertexQ_Index & c_iVertexQ_Mask ].Value;
		
		xyz [ 4 ].Value = GPURegsGp.XYZ2.Value;
		uv [ 4 ].Value = GPURegsGp.UV.Value;
		st [ 4 ].Value = GPURegsGp.ST.Value;
		rgbaq [ 4 ].Value = GPURegsGp.RGBAQ.Value;
		f [ 4 ].Value = GPURegsGp.FOG;
	}

	// set coords as a vertex
	xyz [ lIndex ].Value = GPURegsGp.XYZ2.Value;
	uv [ lIndex ].Value = GPURegsGp.UV.Value;
	st [ lIndex ].Value = GPURegsGp.ST.Value;
	rgbaq [ lIndex ].Value = GPURegsGp.RGBAQ.Value;
	f [ lIndex ].Value = GPURegsGp.FOG;
	
	rgbaq_Current.Value = GPURegsGp.RGBAQ.Value;
	
	lVertexQ_Index++;
	lVertexCount++;
}


void GPU::DrawingKick ()
{
	// array for vertex index before drawing kick for each primitive
	static const u32 c_ulPrimDrawKick [] = { 1, 2, 2, 3, 3, 3, 2, 0 };
	
#ifdef INLINE_DEBUG_DRAWKICK
	debug << "\r\nDrawKick: " << hex << " VertexCount=" << lVertexCount << " PRIM=" << GPURegsGp.PRIM.Value << " Needed=" << c_ulPrimDrawKick [ GPURegsGp.PRIM.Value & 0x7 ];
#endif

	if ( lVertexCount >= c_ulPrimDrawKick [ GPURegsGp.PRIM.Value & 0x7 ] )
	{
		DrawPrimitive ();
	}
}


// need to be able to reset primitive vertex count for some primitives
void GPU::ResetPrimitive ()
{
	// array for vertex index before drawing kick for each primitive
	static const u32 c_ulPrimDrawKick [] = { 1, 2, 2, 3, 3, 3, 2, 0 };
	
#ifdef INLINE_DEBUG_RESET_PRIMITIVE
	debug << "\r\nDrawKick: " << hex << " VertexCount=" << lVertexCount << " PRIM=" << GPURegsGp.PRIM.Value << " Needed=" << c_ulPrimDrawKick [ GPURegsGp.PRIM.Value & 0x7 ];
#endif

	if ( lVertexCount >= c_ulPrimDrawKick [ GPURegsGp.PRIM.Value & 0x7 ] )
	{
		switch ( GPURegsGp.PRIM.Value & 0x7 )
		{
			case 1:
			case 3:
			case 6:
				lVertexCount = 0;
				break;
		}
	}
}


void GPU::DrawPrimitive ()
{
	static const char* c_sPrimNames [] = { "Point", "Line", "Line Strip", "Triangle", "Triangle Strip", "Triangle Fan", "Sprite", "Reserved" };
	
	u32 Coord0, Coord1, Coord2;

	// draw the primitive
	
#ifdef INLINE_DEBUG_PRIMITIVE
	debug << "\r\nDrawPrimitive: " << c_sPrimNames [ GPURegsGp.PRIM.Value & 0x7 ];
#endif

	// update the primitive count (for debugging)
	Primitive_Count++;
	
	Coord0 = ( lVertexQ_Index - 1 ) & c_iVertexQ_Mask;
	Coord1 = ( lVertexQ_Index - 2 ) & c_iVertexQ_Mask;
	Coord2 = ( lVertexQ_Index - 3 ) & c_iVertexQ_Mask;

	switch ( GPURegsGp.PRIM.Value & 0x7 )
	{
		// Point
		case 0:
		
			// should just make a call to "DrawPoint" here
			DrawPoint ( Coord0 );
			break;
			
		// Line
		case 1:
			// note: if it turns out that more lines/objects can be drawn, can also set vertex count to zero here
			
			// line is being drawn from Coord1->Coord0
			DrawLine ( Coord1, Coord0 );
			
			// need to set vertex count back to zero unless it is a strip/fan
			lVertexCount = 0;
			break;
		
		// Line Strip
		case 2:
		
			// only need the last two coords for line strip
			//lVertexQ_Index &= 1;
			
			DrawLine ( Coord1, Coord0 );
			break;
		
		// Triangle
		case 3:
			// order of the coordinates shouldn't matter for triangles
			DrawTriangle ( Coord2, Coord1, Coord0 );
			
			// need to set vertex count back to zero unless it is a strip/fan
			lVertexCount = 0;
			break;
		
		// Triangle Strip
		case 4:
		
			// only need the last three coords for the triangle in strip
			//if ( lVertexQ_Index > 2 ) lVertexQ_Index = 0;
			
			DrawTriangle ( Coord2, Coord1, Coord0 );
			break;
			
		// Triangle Fan
		case 5:
		
			// only need the first coord and the last two for triangle fan
			//if ( lVertexQ_Index > 2 ) lVertexQ_Index = 1;
			
			//DrawTriangleFan ();
			
			// the first coordinate should be saved at the edge of array by vertex kick function
			DrawTriangle ( 4, Coord1, Coord0 );
			break;
			
		// Sprite
		case 6:
			// the starting point is Coord1 here, ending point is Coord0
			DrawSprite ( Coord1, Coord0 );
			
			// need to set vertex count back to zero unless it is a strip/fan
			lVertexCount = 0;
			break;
			
		// Reserved
		case 7:
#ifdef INLINE_DEBUG_PRIMITIVE_RESERVED
	debug << "\r\nALERT: Reserved Primitive";
#endif

#ifdef VERBOSE_PRIMITIVE_RESERVED
			cout << "hps2x64 ALERT: GPU: Trying to draw reserved primitive.";
#endif

			break;
	}
}


double GPU::GetCycles_SinceLastPixel ()
{
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a pixel
	return fmod ( (double) (*_DebugCycleCount), dCyclesPerPixel );
#else
	return RMOD ( (double) (*_DebugCycleCount), dCyclesPerPixel, dPixelsPerCycle );
#endif
}

double GPU::GetCycles_SinceLastPixel ( double dAtCycle )
{
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	return fmod ( dAtCycle, dCyclesPerPixel );
#else
	return RMOD ( dAtCycle, dCyclesPerPixel, dPixelsPerCycle );
#endif
}



double GPU::GetCycles_SinceLastHBlank ()
{
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	return fmod ( ( (double) (*_DebugCycleCount) ) + dHBlankArea_Cycles, dCyclesPerScanline );
#else
	return RMOD ( ( (double) (*_DebugCycleCount) ) + dHBlankArea_Cycles, dCyclesPerScanline, dScanlinesPerCycle );
#endif
}

double GPU::GetCycles_SinceLastHBlank ( double dAtCycle )
{
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	return fmod ( dAtCycle + dHBlankArea_Cycles, dCyclesPerScanline );
#else
	return RMOD ( dAtCycle + dHBlankArea_Cycles, dCyclesPerScanline, dScanlinesPerCycle );
#endif
}


double GPU::GetCycles_SinceLastVBlank ()
{
	double dOffsetFromVBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromVBlankStart = fmod ( ( (double) (*_DebugCycleCount) ) + dVBlank1Area_Cycles, dCyclesPerFrame );
#else
	dOffsetFromVBlankStart = RMOD ( ( (double) (*_DebugCycleCount) ) + dVBlank1Area_Cycles, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	if ( dOffsetFromVBlankStart >= dCyclesPerField1 ) dOffsetFromVBlankStart -= dCyclesPerField1;
	
	return dOffsetFromVBlankStart;
}

double GPU::GetCycles_SinceLastVBlank ( double dAtCycle )
{
	double dOffsetFromVBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromVBlankStart = fmod ( dAtCycle + dVBlank1Area_Cycles, dCyclesPerFrame );
#else
	dOffsetFromVBlankStart = RMOD ( dAtCycle + dVBlank1Area_Cycles, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	if ( dOffsetFromVBlankStart >= dCyclesPerField1 ) dOffsetFromVBlankStart -= dCyclesPerField1;
	
	return dOffsetFromVBlankStart;
}


double GPU::GetCycles_ToNextPixel ()
{
	double dOffsetFromPixelStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a pixel
	dOffsetFromPixelStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerPixel );
#else
	dOffsetFromPixelStart = RMOD ( (double) (*_DebugCycleCount), dCyclesPerPixel, dPixelsPerCycle );
#endif
	
	return dCyclesPerPixel - dOffsetFromPixelStart;
}


double GPU::GetCycles_ToNextPixel ( double dAtCycle )
{
	double dOffsetFromPixelStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a pixel
	dOffsetFromPixelStart = fmod ( dAtCycle, dCyclesPerPixel );
#else
	dOffsetFromPixelStart = RMOD ( dAtCycle, dCyclesPerPixel, dPixelsPerCycle );
#endif
	
	return dCyclesPerPixel - dOffsetFromPixelStart;
}



double GPU::GetCycles_ToNextHBlank ()
{
	double dOffsetFromHBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	//dOffsetFromScanlineStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerScanline );
	dOffsetFromHBlankStart = fmod ( ( (double) (*_DebugCycleCount) ) + dHBlankArea_Cycles, dCyclesPerScanline );
#else
	dOffsetFromHBlankStart = RMOD ( ( (double) (*_DebugCycleCount) ) + dHBlankArea_Cycles, dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	// return the cycles to the next hblank
	return dCyclesPerScanline - dOffsetFromHBlankStart;
}

double GPU::GetCycles_ToNextHBlank ( double dAtCycle )
{
	double dOffsetFromHBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	//dOffsetFromScanlineStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerScanline );
	dOffsetFromHBlankStart = fmod ( dAtCycle + dHBlankArea_Cycles, dCyclesPerScanline );
#else
	dOffsetFromHBlankStart = RMOD ( dAtCycle + dHBlankArea_Cycles, dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	// return the cycles to the next hblank
	return dCyclesPerScanline - dOffsetFromHBlankStart;
}


double GPU::GetCycles_ToNextVBlank ()
{
	double dOffsetFromFrameStart, dOffsetFromVBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromFrameStart = fmod ( ( (double) (*_DebugCycleCount) ) + dVBlank1Area_Cycles, dCyclesPerFrame );
#else
	dOffsetFromFrameStart = RMOD ( ( (double) (*_DebugCycleCount) ) + dVBlank1Area_Cycles, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	dOffsetFromVBlankStart = dCyclesPerField1 - dOffsetFromFrameStart;
	if ( dOffsetFromVBlankStart <= 0 ) dOffsetFromVBlankStart += dCyclesPerField0;
	
	return dOffsetFromVBlankStart;
}

double GPU::GetCycles_ToNextVBlank ( double dAtCycle )
{
	double dOffsetFromFrameStart, dOffsetFromVBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromFrameStart = fmod ( dAtCycle + dVBlank1Area_Cycles, dCyclesPerFrame );
#else
	dOffsetFromFrameStart = RMOD ( dAtCycle + dVBlank1Area_Cycles, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	dOffsetFromVBlankStart = dCyclesPerField1 - dOffsetFromFrameStart;
	if ( dOffsetFromVBlankStart <= 0 ) dOffsetFromVBlankStart += dCyclesPerField0;
	
	return dOffsetFromVBlankStart;
}




// also need functions to see if currently in hblank or vblank
bool GPU::isHBlank ()
{
	double dOffsetFromHBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	//dOffsetFromScanlineStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerScanline );
	dOffsetFromHBlankStart = fmod ( ( (double) (*_DebugCycleCount) ) + dHBlankArea_Cycles, dCyclesPerScanline );
#else
	dOffsetFromHBlankStart = RMOD ( ( (double) (*_DebugCycleCount) ) + dHBlankArea_Cycles, dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	return ( dOffsetFromHBlankStart < dHBlankArea_Cycles );
}

bool GPU::isHBlank ( double dAtCycle )
{
	double dOffsetFromHBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	//dOffsetFromScanlineStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerScanline );
	dOffsetFromHBlankStart = fmod ( dAtCycle + dHBlankArea_Cycles, dCyclesPerScanline );
#else
	dOffsetFromHBlankStart = RMOD ( dAtCycle + dHBlankArea_Cycles, dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	return ( dOffsetFromHBlankStart < dHBlankArea_Cycles );
}


bool GPU::isVBlank ()
{
	double dOffsetFromVBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromVBlankStart = fmod ( ( (double) (*_DebugCycleCount) ) + dVBlank1Area_Cycles, dCyclesPerFrame );
#else
	dOffsetFromVBlankStart = RMOD ( ( (double) (*_DebugCycleCount) ) + dVBlank1Area_Cycles, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	if ( dOffsetFromVBlankStart >= dCyclesPerField1 ) return ( ( dOffsetFromVBlankStart - dCyclesPerField1 ) < dVBlank0Area_Cycles );

	return ( dOffsetFromVBlankStart < dVBlank1Area_Cycles );
	
	//return ( ( lScanline & ~1 ) == lVBlank );
}

bool GPU::isVBlank ( double dAtCycle )
{
	double dOffsetFromVBlankStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromVBlankStart = fmod ( dAtCycle + dVBlank1Area_Cycles, dCyclesPerFrame );
#else
	dOffsetFromVBlankStart = RMOD ( dAtCycle + dVBlank1Area_Cycles, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	if ( dOffsetFromVBlankStart >= dCyclesPerField1 ) return ( ( dOffsetFromVBlankStart - dCyclesPerField1 ) < dVBlank0Area_Cycles );

	return ( dOffsetFromVBlankStart < dVBlank1Area_Cycles );
}




u64 GPU::GetScanline_Count ()
{
	// *** divide ***
	return (u64) ( ( (double) ( *_DebugCycleCount ) ) / dCyclesPerScanline );
}


u64 GPU::GetScanline_Number ()
{
	u64 Scanline_Number;
	
	// *** divide ***
	//return ( ( (u64) ( ( *_DebugCycleCount ) / dCyclesPerScanline ) ) % ( (u64) lMaxScanline ) );
	Scanline_Number = ( ( (u64) ( ( *_DebugCycleCount ) / dCyclesPerScanline ) ) % ( (u64) lMaxScanline ) );
	
	// check if we are in field 0 or 1
	if ( Scanline_Number < ScanlinesPerField0 )
	{
		// field 0 //
		return ( Scanline_Number << 1 );
	}
	else
	{
		// field 1 //
		return 1 + ( ( Scanline_Number - ScanlinesPerField0 ) << 1 );
	}
}


double GPU::GetScanline_Start ()
{
//#ifdef USE_DIVIDE_GCC
//	// *** divide ***
//	return ( ( (double) ( *_DebugCycleCount ) ) / dCyclesPerScanline );
//#else
	//return ( ( (double) ( *_DebugCycleCount ) ) * dScanlinesPerCycle );
	return ( ( (u64) ( ( (double) ( *_DebugCycleCount ) ) * dScanlinesPerCycle ) ) * dCyclesPerScanline );
//#endif
}



double GPU::GetCycles_ToNextScanlineStart ()
{
#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::GetCycles_ToNextScanline";
#endif

	double dOffsetFromScanlineStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	dOffsetFromScanlineStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerScanline );
#else
	dOffsetFromScanlineStart = RMOD ( (double) (*_DebugCycleCount), dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	// return the offset to the start of the next scanline
	return dCyclesPerScanline - dOffsetFromScanlineStart;
}

double GPU::GetCycles_ToNextScanlineStart ( double dAtCycle )
{
#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::GetCycles_ToNextScanline";
#endif

	double dOffsetFromScanlineStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	dOffsetFromScanlineStart = fmod ( dAtCycle, dCyclesPerScanline );
#else
	dOffsetFromScanlineStart = RMOD ( dAtCycle, dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	// return the offset to the start of the next scanline
	return dCyclesPerScanline - dOffsetFromScanlineStart;
}



double GPU::GetCycles_ToNextFieldStart ()
{
#ifdef INLINE_DEBUG_DRAWSTART
	debug << "\r\nGPU::GetCycles_ToNextFieldStart";
#endif

	double dOffsetFromFrameStart, dOffsetFromFieldStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromFrameStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerFrame );
#else
	dOffsetFromFrameStart = RMOD ( (double) (*_DebugCycleCount), dCyclesPerFrame, dFramesPerCycle );
#endif
	
	dOffsetFromFieldStart = dCyclesPerField0 - dOffsetFromFrameStart;
	if ( dOffsetFromFieldStart <= 0 ) dOffsetFromFieldStart += dCyclesPerField1;
	
	return dOffsetFromFieldStart;
}

double GPU::GetCycles_ToNextFieldStart ( double dAtCycle )
{
#ifdef INLINE_DEBUG_DRAWSTART
	debug << "\r\nGPU::GetCycles_ToNextFieldStart";
#endif

	double dOffsetFromFrameStart, dOffsetFromFieldStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromFrameStart = fmod ( dAtCycle, dCyclesPerFrame );
#else
	dOffsetFromFrameStart = RMOD ( dAtCycle, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	dOffsetFromFieldStart = dCyclesPerField0 - dOffsetFromFrameStart;
	if ( dOffsetFromFieldStart <= 0 ) dOffsetFromFieldStart += dCyclesPerField1;
	
	return dOffsetFromFieldStart;
}


double GPU::GetCycles_SinceLastScanlineStart ()
{
#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::GetCycles_ToNextScanline";
#endif

	double dOffsetFromScanlineStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	dOffsetFromScanlineStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerScanline );
#else
	dOffsetFromScanlineStart = RMOD ( (double) (*_DebugCycleCount), dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	// return the offset to the start of the next scanline
	return dOffsetFromScanlineStart;
}

double GPU::GetCycles_SinceLastScanlineStart ( double dAtCycle )
{
#ifdef INLINE_DEBUG_EVENT
	debug << "\r\nGPU::GetCycles_ToNextScanline";
#endif

	double dOffsetFromScanlineStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// get offset from start of scanline
	dOffsetFromScanlineStart = fmod ( dAtCycle, dCyclesPerScanline );
#else
	dOffsetFromScanlineStart = RMOD ( dAtCycle, dCyclesPerScanline, dScanlinesPerCycle );
#endif
	
	// return the offset to the start of the next scanline
	return dOffsetFromScanlineStart;
}



double GPU::GetCycles_SinceLastFieldStart ()
{
#ifdef INLINE_DEBUG_DRAWSTART
	debug << "\r\nGPU::GetCycles_ToNextFieldStart";
#endif

	double dOffsetFromFrameStart, dOffsetFromFieldStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromFrameStart = fmod ( (double) (*_DebugCycleCount), dCyclesPerFrame );
#else
	dOffsetFromFrameStart = RMOD ( (double) (*_DebugCycleCount), dCyclesPerFrame, dFramesPerCycle );
#endif
	
	if ( dOffsetFromFrameStart >= dCyclesPerField0 ) dOffsetFromFrameStart -= dCyclesPerField0;
	
	return dOffsetFromFrameStart;
}

double GPU::GetCycles_SinceLastFieldStart ( double dAtCycle )
{
#ifdef INLINE_DEBUG_DRAWSTART
	debug << "\r\nGPU::GetCycles_ToNextFieldStart";
#endif

	double dOffsetFromFrameStart, dOffsetFromFieldStart;
	
#ifdef USE_DIVIDE_GCC
	// *** divide ***
	// modulo cycle count with cycles in a frame (2 fields)
	dOffsetFromFrameStart = fmod ( dAtCycle, dCyclesPerFrame );
#else
	dOffsetFromFrameStart = RMOD ( dAtCycle, dCyclesPerFrame, dFramesPerCycle );
#endif
	
	if ( dOffsetFromFrameStart >= dCyclesPerField0 ) dOffsetFromFrameStart -= dCyclesPerField0;
	
	return dOffsetFromFrameStart;
}







void GPU::UpdateRaster_VARS ( void )
{
#ifdef INLINE_DEBUG_VARS
	debug << "\r\n->UpdateRaster_VARS";
#endif

	u32 HBlank_PixelCount;
	bool SettingsChange;
	
	// assume settings will not change
	SettingsChange = false;

	// if the display settings are going to change, then need to mark cycle at which they changed
	if ( HBlank_X != HBlank_X_LUT [ 0 ] ||
		lVBlank != VBlank_Y_LUT [ 0 ] ||
		Raster_XMax != Raster_XMax_LUT [ 0 ] [ 0 ] ||
		lMaxScanline != Raster_YMax_LUT [ 0 ] )
	{
#ifdef INLINE_DEBUG_VARS
	debug << "\r\nChange; StartCycle=" << dec << *_DebugCycleCount;
#endif

		// ***TODO*** need to update timers before clearing the pixel counts //
		
		// update timers using settings from before they change
		// *todo* really only need to update timers if they are pulling signal from hblank
		Timers::_TIMERS->UpdateTimer ( 0 );
		Timers::_TIMERS->UpdateTimer ( 1 );
		Timers::_TIMERS->UpdateTimer ( 2 );
		Timers::_TIMERS->UpdateTimer ( 3 );
		
		// at end of routine, calibrate timers

		//RasterChange_StartCycle = *_DebugCycleCount;
		SettingsChange = true;
	}


	HBlank_X = HBlank_X_LUT [ 0 ];

#ifdef INLINE_DEBUG_VARS
	debug << "; HBlank_X = " << dec << HBlank_X;
#endif

	lVBlank = VBlank_Y_LUT [ 0 ];

#ifdef INLINE_DEBUG_VARS
	debug << "; lVBlank = " << lVBlank;
#endif

	Raster_XMax = Raster_XMax_LUT [ 0 ] [ 0 ];

#ifdef INLINE_DEBUG_VARS
	debug << "; Raster_XMax = " << Raster_XMax;
#endif

	lMaxScanline = Raster_YMax_LUT [ 0 ];

#ifdef INLINE_DEBUG_VARS
	debug << "; lMaxScanline = " << lMaxScanline;
#endif

	CyclesPerPixel_INC = CyclesPerPixel_INC_Lookup [ 0 ] [ 0 ];
	dCyclesPerPixel = CyclesPerPixel_Lookup [ 0 ] [ 0 ];
	dPixelsPerCycle = PixelsPerCycle_Lookup [ 0 ] [ 0 ];
	
	// check if ntsc or pal
	if ( 1 )
	{
		// is PAL //
		dCyclesPerScanline = PAL_CyclesPerScanline;
		dCyclesPerFrame = PAL_CyclesPerFrame;
		dCyclesPerField0 = PAL_CyclesPerField_Even;
		dCyclesPerField1 = PAL_CyclesPerField_Odd;
		
		dScanlinesPerCycle = PAL_ScanlinesPerCycle;
		dFramesPerCycle = PAL_FramesPerCycle;
		dFieldsPerCycle0 = PAL_FieldsPerCycle_Even;
		dFieldsPerCycle1 = PAL_FieldsPerCycle_Odd;

		
		dDisplayArea_Cycles = PAL_DisplayAreaCycles;
		dVBlank0Area_Cycles = PAL_CyclesPerVBlank_Even;
		dVBlank1Area_Cycles = PAL_CyclesPerVBlank_Odd;
		
		// also need the scanlines per field
		ScanlinesPerField0 = PAL_ScanlinesPerField_Even;
		ScanlinesPerField1 = PAL_ScanlinesPerField_Odd;
	}
	else
	{
		// is NTSC //
		dCyclesPerScanline = NTSC_CyclesPerScanline;
		dCyclesPerFrame = NTSC_CyclesPerFrame;
		dCyclesPerField0 = NTSC_CyclesPerField_Even;
		dCyclesPerField1 = NTSC_CyclesPerField_Odd;
		
		dScanlinesPerCycle = NTSC_ScanlinesPerCycle;
		dFramesPerCycle = NTSC_FramesPerCycle;
		dFieldsPerCycle0 = NTSC_FieldsPerCycle_Even;
		dFieldsPerCycle1 = NTSC_FieldsPerCycle_Odd;
		
		
		dDisplayArea_Cycles = NTSC_DisplayAreaCycles;
		dVBlank0Area_Cycles = NTSC_CyclesPerVBlank_Even;
		dVBlank1Area_Cycles = NTSC_CyclesPerVBlank_Odd;
		
		// also need the scanlines per field
		ScanlinesPerField0 = NTSC_ScanlinesPerField_Even;
		ScanlinesPerField1 = NTSC_ScanlinesPerField_Odd;
	}

	// get number of pixels in hblank area
	HBlank_PixelCount = Raster_XMax - HBlank_X;
	
	// multiply by cycles per pixel
	dHBlankArea_Cycles = ( (double) HBlank_PixelCount ) * dCyclesPerPixel;

	
#ifdef INLINE_DEBUG_VARS
	debug << "; CyclesPerPixel_INC = " << CyclesPerPixel_INC;
#endif

	// the Display_Width is the HBlank_X
	Display_Width = HBlank_X;
	
	// the Display_Height is the lVBlank if interlaced, otherwise it is lVBlank/2
	if ( 0 /*GPU_CTRL_Read.ISINTER*/ )
	{
		Display_Height = lVBlank;
	}
	else
	{
		Display_Height = ( lVBlank >> 1 );
	}
	

	// check if the settings changed
	if ( SettingsChange )
	{
		// settings changed //
		
		// get the next event for gpu if settings change
		GetNextEvent ();
		
		// calibrate timers 0,1,2
		// *todo* really only need to update timers if they are pulling signal from hblank
		Timers::_TIMERS->CalibrateTimer ( 0 );
		Timers::_TIMERS->CalibrateTimer ( 1 );
		Timers::_TIMERS->CalibrateTimer ( 2 );
		Timers::_TIMERS->CalibrateTimer ( 3 );
		
		// if doing calibrate, then also must do update of next event
		// *todo* really only need to update timers if they are pulling signal from hblank
		Timers::_TIMERS->Get_NextEvent ( 0 );
		Timers::_TIMERS->Get_NextEvent ( 1 );
		Timers::_TIMERS->Get_NextEvent ( 2 );
		Timers::_TIMERS->Get_NextEvent ( 3 );
	}

#ifdef INLINE_DEBUG_VARS
	debug << "->UpdateRaster_VARS";
#endif
}


// send circular buffer data with draw commands to gpu hardware
// starting from ullReadIdx, up to but not including ullWriteIdx
//#ifdef ENABLE_HWPIXEL_INPUT
//void GPU::FlushToHardware ( u64 ullReadIdx, u64 ullWriteIdx, u64 ullReadPixelIdx, u64 ullWritePixelIdx )
//#else
//void GPU::FlushToHardware ( u64 ullReadIdx, u64 ullWriteIdx )
//#endif
void GPU::FlushToHardware ( u64 ullReadIdx, u64 ullWriteIdx, u64 ullReadPixelIdx, u64 ullWritePixelIdx )
{
	u64 ullCircularBufferEdge;
	u64 ullSourceIndex;
	u64 ullSourceIndex2;
	u64 ullTransferByteCount;
	u64 ullTransferByteCount2;
	int i;

	// check to make sure there is data to send to gpu hardware
	if ( ullReadIdx >= ullWriteIdx )
	{
		// nothing to send to gpu hardware //
		return;
	}

	// make sure there is no buffer overflow
	if ( ( ullWriteIdx - ullReadIdx ) > c_ulHwInputBuffer_Size )
	{
		cout << "\nhps2x64: GPU: ERROR: draw buffer overflow: " << dec << ( ullWriteIdx - ullReadIdx );
		return;
	}

	// make sure pixel buffer has no overflow
	if ( ( ullWritePixelIdx - ullReadPixelIdx ) > c_ullPixelInBuffer_Size )
	{
		cout << "\nhps2x64: GPU: ERROR: pixel buffer overflow: " << dec << ( ullWritePixelIdx - ullReadPixelIdx );
		return;
	}


	DisplayOutput_Window->OpenGL_MakeCurrentWindow ();


	// first flush data/pixel input buffer //
#ifdef ENABLE_HWPIXEL_INPUT

	ullCircularBufferEdge = ( ( ullReadPixelIdx | c_ullPixelInBuffer_Mask ) + 1 );

	glBindBuffer ( GL_COPY_READ_BUFFER, ssbo_sinputpixels );
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, ssbo_inputpixels);
	if( ullWritePixelIdx <= ullCircularBufferEdge )
	{
		// first copy data into hardware buffer
		ullSourceIndex = ( ullReadPixelIdx & c_ullPixelInBuffer_Mask ) << 0;
		ullTransferByteCount = 1 * 4 * ( ullWritePixelIdx - ullReadPixelIdx );

		memcpy( & p_ulHwPixelInBuffer32 [ ullSourceIndex ], & ulPixelInBuffer32 [ ullSourceIndex ], ullTransferByteCount );

		// now dispatch a copy command
		glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, ullSourceIndex << 2, 0, ullTransferByteCount );

	}
	else
	{
		// circular buffer is wrapping around //

		//cout << "***BUFFER WRAP***";

		ullSourceIndex = ( ullReadPixelIdx & c_ullPixelInBuffer_Mask ) << 0;
		ullTransferByteCount = 1 * 4 * ( ullCircularBufferEdge - ullReadPixelIdx );

		ullSourceIndex2 = 0;
		ullTransferByteCount2 = 1 * 4 * ( ullWritePixelIdx & c_ullPixelInBuffer_Mask );

		// first copy the data in
		memcpy( & p_ulHwPixelInBuffer32 [ ullSourceIndex ], & ulPixelInBuffer32 [ ullSourceIndex ], ullTransferByteCount );
		memcpy( & p_ulHwPixelInBuffer32 [ ullSourceIndex2 ], & ulPixelInBuffer32 [ ullSourceIndex2 ], ullTransferByteCount2 );

		// now copy the buffers
		glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, ullSourceIndex << 2, 0, ullTransferByteCount );
		glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, ullSourceIndex2 << 2, ullTransferByteCount, ullTransferByteCount2 );

	}
#endif


/*
#ifdef ALLOW_OPENCL_PS2_TEST_CLUT
	// copy clut
	glBindBuffer ( GL_COPY_READ_BUFFER, ssbo_sclut );
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssbo_clut);
	//memcpy( p_uHwClutData32, _GPU->InternalCLUT, sizeof(_GPU->InternalCLUT) );
	for ( i = 0; i < 512; i++ )
	{
		p_uHwClutData32 [ i ] = ((u16*)_GPU->InternalCLUT)[ i ];
	}

	glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, sizeof(_GPU->InternalCLUT) * 2 );

	// copy cbpx
	glBindBuffer ( GL_COPY_READ_BUFFER, ssbo_scbpx );
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssbo_cbpx);

	memcpy( p_uHwCbpxData32, _GPU->CBPX, sizeof(_GPU->CBPX) );

	glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, sizeof(_GPU->CBPX) );
#endif
*/


	//cout << "\nSending Flush to gpu render. ReadIdx= " << dec << ulInputBuffer_ReadIndex << " WriteIdx= " << ulInputBuffer_WriteIndex;
	ullCircularBufferEdge = ( ( ullReadIdx | c_ulHwInputBuffer_Mask ) + 1 );

	glBindBuffer ( GL_COPY_READ_BUFFER, ssbo_sinputdata );
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo1);
	if( ullWriteIdx <= ullCircularBufferEdge )
	{
		// first copy data into hardware buffer
		ullSourceIndex = ( ullReadIdx & c_ulHwInputBuffer_Mask ) << 5;
		ullTransferByteCount = 32 * 8 * ( ullWriteIdx - ullReadIdx );

		memcpy( & p_uHwInputData32 [ ullSourceIndex << 1 ], & pHwInputBuffer64 [ ullSourceIndex ], ullTransferByteCount );

		// now dispatch a copy command
		glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, ullSourceIndex << 3, 0, ullTransferByteCount );

		glDispatchCompute(1, 1, 1);
		//glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
		glMemoryBarrier( GL_ALL_BARRIER_BITS );
	}
	else
	{
		// circular buffer is wrapping around //

		//cout << "***BUFFER WRAP***";

		ullSourceIndex = ( ullReadIdx & c_ulHwInputBuffer_Mask ) << 5;
		ullTransferByteCount = 32 * 8 * ( ullCircularBufferEdge - ullReadIdx );

		ullSourceIndex2 = 0;
		ullTransferByteCount2 = 32 * 8 * ( ullWriteIdx & c_ulHwInputBuffer_Mask );

		// first copy the data in
		memcpy( & p_uHwInputData32 [ ullSourceIndex << 1 ], & pHwInputBuffer64 [ ullSourceIndex ], ullTransferByteCount );
		memcpy( & p_uHwInputData32 [ ullSourceIndex2 << 1 ], & pHwInputBuffer64 [ ullSourceIndex2 ], ullTransferByteCount2 );

		// now copy the buffers
		glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, ullSourceIndex << 3, 0, ullTransferByteCount );
		glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, ullSourceIndex2 << 3, ullTransferByteCount, ullTransferByteCount2 );

		glDispatchCompute(1, 1, 1);
		//glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
		glMemoryBarrier( GL_ALL_BARRIER_BITS );
	}

	//glFlush ();



	DisplayOutput_Window->OpenGL_ReleaseWindow ();

}


void GPU::Draw_Screen ()
{
	static const int c_iVisibleArea_StartY [] = { c_iVisibleArea_StartY_Pixel_NTSC, c_iVisibleArea_StartY_Pixel_PAL };
	static const int c_iVisibleArea_EndY [] = { c_iVisibleArea_EndY_Pixel_NTSC, c_iVisibleArea_EndY_Pixel_PAL };
	
	
#ifdef INLINE_DEBUG_DRAW_SCREEN
	debug << "\r\n***Frame Draw***";
	debug << " PMode=" << hex << GPURegs0.PMODE;
	debug << " FBOffset1/2048=" << hex << GPURegs0.DISPFB1.FBP;
	debug << " FBOffset2/2048=" << hex << GPURegs0.DISPFB2.FBP;
	debug << " FBWidth1/64 (for screen output/in pixels)=" << dec << GPURegs0.DISPFB1.FBW;
	debug << " FBWidth2/64 (for screen output/in pixels)=" << dec << GPURegs0.DISPFB2.FBW;
	debug << " FBHeight1-1 (for screen output/in pixels)=" << dec << GPURegs0.DISPLAY1.DH;
	debug << " FBHeight2-1 (for screen output/in pixels)=" << dec << GPURegs0.DISPLAY2.DH;
	debug << " XPixel1=" << dec << GPURegs0.DISPFB1.DBX;
	debug << " YPixel1=" << dec << GPURegs0.DISPFB1.DBY;
	debug << " XPixel2=" << dec << GPURegs0.DISPFB2.DBX;
	debug << " YPixel2=" << dec << GPURegs0.DISPFB2.DBY;
	debug << " PixelFormat=" << PixelFormat_Names [ GPURegs0.DISPFB2.PSM ];
#endif

	int x, y;
	
	u32* buf_ptr;
	u32 *buf_ptr32;
	u16 *buf_ptr16;
	s32 draw_buffer_offset;
	s32 draw_width, draw_height;
	s32 start_x, start_y;
	s32 Index = 0;
	
	u32 Pixel16, Pixel32;
	u32 PixelFormat;
	

	u64 *inputdata_ptr;
	
#ifdef _ENABLE_AVX2

	__m256i vZero;
	vZero = _mm256_xor_si256( vZero, vZero );
	
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 (7, 6, 5, 4, 3, 2, 1, 0);
	
	__m256i vXInc;
	vXInc = _mm256_set1_epi32 ( 8 );
	
	__m256i vPixel32;
	__m256i vX, vY;
	__m256i vWidth, vOffset32;

	__m256i vEnableAll;
	vEnableAll = _mm256_set1_epi32 ( -1 );
#else
	
#ifdef _ENABLE_SSE41
	__m128i vZero;
	vZero = _mm_xor_si128( vZero, vZero );
	
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 (3, 2, 1, 0);
	
	__m128i vXInc;
	vXInc = _mm_set1_epi32 ( 4 );
	
	__m128i vPixel32;
	__m128i vX, vY;
	__m128i vWidth, vOffset32;
#endif

#endif


	// make sure that framebuffer has some width and height to it before drawing it
	if ( ( GPURegs0.PMODE & 1 ) && GPURegs0.DISPFB1.FBW && GPURegs0.DISPLAY1.DH )
	{
#ifdef INLINE_DEBUG_DRAW_SCREEN
	debug << " DISPLAY1";
#endif

		// display 1 //
		
		draw_buffer_offset = GPURegs0.DISPFB1.FBP << 11;
		
		//buf_ptr = & ( RAM32 [ GPURegs0.DISPFB1.FBP >> 4 ] );
		buf_ptr = & ( RAM32 [ draw_buffer_offset ] );
		
		draw_width = GPURegs0.DISPFB1.FBW << 6;
		draw_height = GPURegs0.DISPLAY1.DH + 1;
		start_x = GPURegs0.DISPFB1.DBX;
		start_y = GPURegs0.DISPFB1.DBY;
		
		buf_ptr32 = buf_ptr;
		buf_ptr16 = (u16*) buf_ptr;
		
		PixelFormat = GPURegs0.DISPFB1.PSM;
		
	}
	else if ( GPURegs0.PMODE & 2 )
	{
#ifdef INLINE_DEBUG_DRAW_SCREEN
	debug << " DISPLAY2";
#endif

		// display 2 //
		
		draw_buffer_offset = GPURegs0.DISPFB2.FBP << 11;
		
		//buf_ptr = & ( RAM32 [ GPURegs0.DISPFB2.FBP >> 4 ] );
		buf_ptr = & ( RAM32 [ draw_buffer_offset ] );
		
		draw_width = GPURegs0.DISPFB2.FBW << 6;
		draw_height = GPURegs0.DISPLAY2.DH + 1;
		start_x = GPURegs0.DISPFB2.DBX;
		start_y = GPURegs0.DISPFB2.DBY;
		
		buf_ptr32 = buf_ptr;
		buf_ptr16 = (u16*) buf_ptr;
		
		PixelFormat = GPURegs0.DISPFB2.PSM;
		
	}
	else
	{
		// ???
		// todo: probably should black out screen in this case ??
		return;
	}

	// draw height can only be a maximum of 1024
	draw_height = ( ( draw_height > c_iScreen_MaxHeight ) ? c_iFrameBuffer_DisplayHeight : draw_height );
	
	// draw width can only be a maximum of 1024
	draw_width = ( ( draw_width > c_iScreen_MaxWidth ) ? c_iFrameBuffer_DisplayWidth : draw_width );


#ifdef ENABLE_PIXELBUF_INTERLACING
	// if set to read every line, then half draw height for now
	// *todo* need to take into account whether interlaced or not
	// comment this out to show correct screen at top for now
	//if ( GPURegs0.SMODE2.FFMD ) draw_height >>= 1;
	// check if this is set to read every line
	if ( GPURegs0.SMODE2.FFMD && GPURegs0.SMODE2.INTER )
	{
		// only draw half the draw height for now
		draw_height >>= 1;
		
		// set to read every line, so need to skip lines when writing to pixel buffer for interlacing
		if ( ( lScanline & 1 ) )
		{
			Index += draw_width;
		}
	}
#endif



#ifdef ALLOW_OPENCL_PS2_DRAWSCREEN
	//bEnable_OpenCL = true;
	if ( bEnable_OpenCL )
	{
		//cout << "\nopengl draw screen";

		// copy the vram into the gpu for testing
		//Copy_VRAM_toGPU ();

		inputdata_ptr = & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];

		inputdata_ptr [ 0 ] = _GPU->GPURegs0.PMODE;
		inputdata_ptr [ 1 ] = _GPU->GPURegs0.DISPFB1.Value;
		inputdata_ptr [ 2 ] = _GPU->GPURegs0.DISPFB2.Value;
		inputdata_ptr [ 3 ] = _GPU->GPURegs0.DISPLAY1.Value;
		inputdata_ptr [ 4 ] = _GPU->GPURegs0.DISPLAY2.Value;
		inputdata_ptr [ 5 ] = _GPU->GPURegs0.SMODE2.Value;
		inputdata_ptr [ 13 ] = _GPU->lScanline;

		// put in command
		inputdata_ptr [ 15 ] = 0xfe000000ull;
		inputdata_ptr [ 14 ] = *_DebugCycleCount;

		// commit command
		ulHwInputBuffer_WriteIndex++;


#ifdef ENABLE_OPENCL_PASSIVE_RENDER

#ifdef ENABLE_PIXELBUF_INTERLACING	
	// for now, if interlaced, need to put the draw_height back
	if ( GPURegs0.SMODE2.FFMD && GPURegs0.SMODE2.INTER )
	{
		// only draw half the draw height for now
		draw_height <<= 1;
		
	}
#endif


		DisplayOutput_Window->OpenGL_MakeCurrentWindow ();

		//cout << "\ndrawing screen. WriteIdx=" << dec << ulHwInputBuffer_WriteIndex;

		glBlitFramebuffer( 0, 0, draw_width, draw_height, 0, 0, MainProgramWindow_Width, MainProgramWindow_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST );

		// update screen
		DisplayOutput_Window->FlipScreen ();

#endif

		// submit all the commands in buffer
		//FlushToHardware( 0, 1 );
		Flush ();

		//cout << "\nscreen drawn.";

	}
	else
#endif
	{
	
	

	

	
#ifdef INLINE_DEBUG_DRAW_SCREEN
	debug << " draw_width=" << dec << draw_width;
	debug << " draw_height=" << dec << draw_height;
	debug << " PixelFormat=" << dec << PixelFormat;
#endif
	
	// draw starting from correct position
	//buf_ptr = buf_ptr [ start_x + ( start_y * draw_width ) ];
	
	// testing
	//start_x = 0;
	//start_y = 0;



	
	//if ( !GPURegs0.DISPFB2.PSM || GPURegs0.DISPFB2.PSM == 1 )
	if ( PixelFormat < 2 )
	{
#ifdef INLINE_DEBUG_DRAW_SCREEN
	debug << " PIXEL32";
#endif

		// 24/32-bit pixels in frame buffer //
		
		//buf_ptr32 = & ( RAM32 [ GPURegs0.DISPFB2.FBP >> 4 ] );
		
		
		
		
		
		// pad on the bottom with zeros if needed for now
		y = 0;
		while ( y < start_y )
		{
#ifdef _ENABLE_AVX2
			for ( x = 0; x < ( draw_width - 7 ); x += 8 )
			{
				//PixelBuffer [ Index++ ] = 0;
				//_mm_storeu_si128 ( (__m128i*) & PixelBuffer [ Index ], vZero );
				_mm256_store_si256 ( (__m256i*) & PixelBuffer [ Index ], vZero );
				Index += 8;
			}

#else
#ifdef _ENABLE_SSE41
			for ( x = 0; x < ( draw_width - 3 ); x += 4 )
			{
				//PixelBuffer [ Index++ ] = 0;
				//_mm_storeu_si128 ( (__m128i*) & PixelBuffer [ Index ], vZero );
				_mm_store_si128 ( (__m128i*) & PixelBuffer [ Index ], vZero );
				Index += 4;
			}
			for ( ; x < draw_width; x++ )
			{
				PixelBuffer [ Index++ ] = 0;
			}
#else
			for ( x = 0; x < draw_width; x++ )
			{
				PixelBuffer [ Index++ ] = 0;
			}
#endif

#endif
			
			y++;

		}


#ifdef _ENABLE_AVX2
		vWidth = _mm256_set1_epi32 ( draw_width );
#else
#ifdef _ENABLE_SSE41
		vWidth = _mm_set1_epi32 ( draw_width );
#endif
#endif

		// copy the pixels into pixel buffer
		for ( y = draw_height - 1; y >= start_y; y-- )
		{
#ifdef _ENABLE_AVX2
			vY = _mm256_set1_epi32 ( y );
			vX = _mm256_set1_epi32 ( start_x );
			vX = _mm256_add_epi32 ( vX, vIdentity );
			for ( x = start_x; x < ( draw_width - 3 ); x += 8 )
			{
				//Pixel32 = buf_ptr32 [ CvtAddrPix32( x, y, draw_width ) ];
				vOffset32 = CvtAddrPix32x8 ( vX, vY, vWidth, vEnableAll );
				
				//vPixel32 = _mm_set_epi32 (
				//	buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 3 ) ],
				//	buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 2 ) ],
				//	buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 1 ) ],
				//	buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 0 ) ]
				//);
				vPixel32 = _mm256_i32gather_epi32 ((int const*) buf_ptr32, vOffset32, 4);
				
				//PixelBuffer [ Index++ ] = Pixel32;
				//_mm_storeu_si128 ( (__m128i*) & PixelBuffer [ Index ], vPixel32 );
				_mm256_store_si256 ( (__m256i*) & PixelBuffer [ Index ], vPixel32 );
				
				vX = _mm256_add_epi32 ( vX, vXInc );
				
				Index += 8;
			}

#else
#ifdef _ENABLE_SSE41
			vY = _mm_set1_epi32 ( y );
			vX = _mm_set1_epi32 ( start_x );
			vX = _mm_add_epi32 ( vX, vIdentity );
			for ( x = start_x; x < ( draw_width - 3 ); x += 4 )
			{
				//Pixel32 = buf_ptr32 [ CvtAddrPix32( x, y, draw_width ) ];
				vOffset32 = CvtAddrPix32x4 ( vX, vY, vWidth, _mm_set1_epi32( -1 ) );
				vPixel32 = _mm_set_epi32 (
					buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 3 ) ],
					buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 2 ) ],
					buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 1 ) ],
					buf_ptr32 [ _mm_extract_epi32 ( vOffset32, 0 ) ]
				);
				
				//PixelBuffer [ Index++ ] = Pixel32;
				//_mm_storeu_si128 ( (__m128i*) & PixelBuffer [ Index ], vPixel32 );
				_mm_store_si128 ( (__m128i*) & PixelBuffer [ Index ], vPixel32 );
				
				vX = _mm_add_epi32 ( vX, vXInc );
				
				Index += 4;
			}
			for ( ; x < draw_width; x++ )
			{
				PixelBuffer [ Index++ ] = buf_ptr32 [ CvtAddrPix32( x, y, draw_width ) ];
			}
#else
			for ( x = start_x; x < draw_width; x++ )
			{
				Pixel32 = buf_ptr32 [ CvtAddrPix32( x, y, draw_width ) ];
				
				PixelBuffer [ Index++ ] = Pixel32;
			}
#endif
#endif
			
			// pad on the right with zeros if needed for now
			x = 0;
			while ( x < start_x )
			{
				PixelBuffer [ Index++ ] = 0;
				x++;
			}
			
#ifdef ENABLE_PIXELBUF_INTERLACING	
			// if reading every other line, only copy every other line to pixel buffer
			if ( GPURegs0.SMODE2.FFMD && GPURegs0.SMODE2.INTER )
			{
				Index += draw_width;
			}
#endif

		}
	}
	else
	{
#ifdef INLINE_DEBUG_DRAW_SCREEN
	debug << " PIXEL16";
#endif

		// assume 16-bit pixels for frame buffer for now //



		//cout << "\nPS2GPU: 16-bit pixel fb format=" << hex << PixelFormat;

		
		//buf_ptr16 = (u16*) ( & ( RAM32 [ GPURegs0.DISPFB2.FBP >> 4 ] ) );
		
		// pad on the bottom with zeros if needed for now
		y = 0;
		while ( y < start_y )
		{
#ifdef _ENABLE_SSE41
			for ( x = 0; x < ( draw_width - 3 ); x += 4 )
			{
				//PixelBuffer [ Index++ ] = 0;
				_mm_storeu_si128 ( (__m128i*) & PixelBuffer [ Index ], vZero );
				Index += 4;
			}
			for ( ; x < draw_width; x++ )
			{
				PixelBuffer [ Index++ ] = 0;
			}
#else
			for ( x = 0; x < draw_width; x++ )
			{
				PixelBuffer [ Index++ ] = 0;
			}
#endif
			
			y++;
		}
		
		//for ( y = start_y + ( draw_height - 1 ); y >= start_y; y-- )
		for ( y = draw_height - 1; y >= start_y; y-- )
		{
			//for ( x = start_x; x < ( start_x + draw_width ); x++ )
			for ( x = start_x; x < draw_width; x++ )
			{
#ifdef ENABLE_DATA_STRUCTURE
				switch ( PixelFormat )
				{
					case 2:
						Pixel16 = buf_ptr16 [ CvtAddrPix16( x, y, draw_width ) ];
						break;
						
					case 0xa:
						Pixel16 = buf_ptr16 [ CvtAddrPix16S( x, y, draw_width ) ];
						break;
				}
#else
				Pixel16 = buf_ptr16 [ x + ( y * draw_width ) ];
#endif

				Pixel32 = ( ( Pixel16 & 0x1f ) << 3 ) | ( ( ( Pixel16 >> 5 ) & 0x1f ) << ( 8 + 3 ) ) | ( ( ( Pixel16 >> 10 ) & 0x1f ) << ( 16 + 3 ) );
				PixelBuffer [ Index++ ] = Pixel32;
			}
			
			// pad on the right with zeros if needed for now
			x = 0;
			while ( x < start_x )
			{
				PixelBuffer [ Index++ ] = 0;
				x++;
			}
			
#ifdef ENABLE_PIXELBUF_INTERLACING	
			// if reading every other line, only copy every other line to pixel buffer
			if ( GPURegs0.SMODE2.FFMD && GPURegs0.SMODE2.INTER )
			{
				Index += draw_width;
			}
#endif

		}
	}

	}	// end 
	
	
#ifdef ENABLE_PIXELBUF_INTERLACING	
	// for now, if interlaced, need to put the draw_height back
	if ( GPURegs0.SMODE2.FFMD && GPURegs0.SMODE2.INTER )
	{
		// only draw half the draw height for now
		draw_height <<= 1;
		
	}
#endif
	
		
	// *** output of pixel buffer to screen *** //

	// make this the current window we are drawing to
	DisplayOutput_Window->OpenGL_MakeCurrentWindow ();

#ifdef ALLOW_OPENCL_PS2_DRAWSCREEN
	if ( _GPU->bEnable_OpenCL )
	{

#ifdef ENABLE_OPENCL_ACTIVE_RENDER

		glBlitFramebuffer( 0, 0, draw_width, draw_height, 0, 0, MainProgramWindow_Width, MainProgramWindow_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST );

		// update screen
		DisplayOutput_Window->FlipScreen ();

#endif

		// this is no longer the current window we are drawing to
		DisplayOutput_Window->OpenGL_ReleaseWindow ();
	}
	else
#endif
	{
	//glPixelZoom ( (float)MainProgramWindow_Width / (float)VisibleArea_Width, (float)MainProgramWindow_Height / (float)VisibleArea_Height );
	glPixelZoom ( (float)MainProgramWindow_Width / (float)draw_width, (float)MainProgramWindow_Height / (float)draw_height );
	//glDrawPixels ( VisibleArea_Width, VisibleArea_Height, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*) PixelBuffer );
	glDrawPixels ( draw_width, draw_height, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*) PixelBuffer );

	// update screen
	DisplayOutput_Window->FlipScreen ();

	// this is no longer the current window we are drawing to
	DisplayOutput_Window->OpenGL_ReleaseWindow ();
	}
	
	
	
	
}


void GPU::Copy_Buffer ( u32* dstbuf, u32* srcbuf, u32 dstbuffer_width, u32 dstbuffer_height, u32 srcbuffer_width, u32 srcbuffer_height, u32 SrcPixelFormat )
{
	
	u32 Pixel, FramePixel;
	s32 Pixel_X, Pixel_Y;
	
	u32 *output_buf;
	
	//srcbuf = & ( RAM32 [ FrameBufferStartOffset32 ] );

	
	FramePixel = 0;
	
	if ( SrcPixelFormat < 2 )
	{
		// 32-bit pixel //
		
		/////////////////////////////////////////////////////////////////
		// Draw contents of frame buffer
		//for ( Pixel_Y = FrameBuffer_Height - 1; Pixel_Y >= 0; Pixel_Y-- )
		for ( Pixel_Y = srcbuffer_height - 1; Pixel_Y >= 0; Pixel_Y-- )
		{
			output_buf = & ( dstbuf [ Pixel_Y * dstbuffer_width ] );
			
			if ( Pixel_Y < dstbuffer_height && Pixel_Y < srcbuffer_height )
			{
				//for ( Pixel_X = 0; Pixel_X < FrameBuffer_Width; Pixel_X++ )
				for ( Pixel_X = 0; ( Pixel_X < dstbuffer_width ) && ( Pixel_X < srcbuffer_width ); Pixel_X++ )
				{
					//Pixel = srcbuf [ Pixel_X + ( Pixel_Y * srcbuffer_width ) ];
					Pixel = srcbuf [ CvtAddrPix32 ( Pixel_X, Pixel_Y, srcbuffer_width ) ];
					
					//PixelBuffer [ FramePixel++ ] = ( ( Pixel & 0x1f ) << ( 3 ) ) | ( ( (Pixel >> 5) & 0x1f ) << ( 3 + 8 ) ) | ( ( (Pixel >> 10) & 0x1f ) << ( 3 + 16 ) );
					*output_buf++ = Pixel & 0xffffff;
				}
			}
		}
	
	}
	else if ( SrcPixelFormat == 2 )
	{
		// 16-bit pixel //
		
		/////////////////////////////////////////////////////////////////
		// Draw contents of frame buffer
		//for ( Pixel_Y = FrameBuffer_Height - 1; Pixel_Y >= 0; Pixel_Y-- )
		for ( Pixel_Y = srcbuffer_height - 1; Pixel_Y >= 0; Pixel_Y-- )
		{
			output_buf = & ( dstbuf [ Pixel_Y * dstbuffer_width ] );
			
			if ( Pixel_Y < dstbuffer_height && Pixel_Y < srcbuffer_height )
			{
				//for ( Pixel_X = 0; Pixel_X < FrameBuffer_Width; Pixel_X++ )
				for ( Pixel_X = 0; ( Pixel_X < dstbuffer_width ) && ( Pixel_X < srcbuffer_width ); Pixel_X++ )
				{
					//Pixel = srcbuf [ Pixel_X + ( Pixel_Y * srcbuffer_width ) ];
					Pixel = ((u16*)srcbuf) [ CvtAddrPix16 ( Pixel_X, Pixel_Y, srcbuffer_width ) ];
					
					// convert 16-bit pixel to 32-bit pixel
					Pixel = ( ( Pixel & 0x7c00 ) << 9 ) | ( ( Pixel & 0x3e0 ) << 6 ) | ( ( Pixel & 0x1f ) << 3 );
					
					//PixelBuffer [ FramePixel++ ] = ( ( Pixel & 0x1f ) << ( 3 ) ) | ( ( (Pixel >> 5) & 0x1f ) << ( 3 + 8 ) ) | ( ( (Pixel >> 10) & 0x1f ) << ( 3 + 16 ) );
					*output_buf++ = Pixel & 0xffffff;
				}
			}
		}
	}
	else
	{
		// 16S-bit pixel //
		
		/////////////////////////////////////////////////////////////////
		// Draw contents of frame buffer
		//for ( Pixel_Y = FrameBuffer_Height - 1; Pixel_Y >= 0; Pixel_Y-- )
		for ( Pixel_Y = srcbuffer_height - 1; Pixel_Y >= 0; Pixel_Y-- )
		{
			output_buf = & ( dstbuf [ Pixel_Y * dstbuffer_width ] );
			
			if ( Pixel_Y < dstbuffer_height && Pixel_Y < srcbuffer_height )
			{
				//for ( Pixel_X = 0; Pixel_X < FrameBuffer_Width; Pixel_X++ )
				for ( Pixel_X = 0; ( Pixel_X < dstbuffer_width ) && ( Pixel_X < srcbuffer_width ); Pixel_X++ )
				{
					//Pixel = srcbuf [ Pixel_X + ( Pixel_Y * srcbuffer_width ) ];
					Pixel = ((u16*)srcbuf) [ CvtAddrPix16S ( Pixel_X, Pixel_Y, srcbuffer_width ) ];
					
					// convert 16-bit pixel to 32-bit pixel
					Pixel = ( ( Pixel & 0x7c00 ) << 9 ) | ( ( Pixel & 0x3e0 ) << 6 ) | ( ( Pixel & 0x1f ) << 3 );
					
					//PixelBuffer [ FramePixel++ ] = ( ( Pixel & 0x1f ) << ( 3 ) ) | ( ( (Pixel >> 5) & 0x1f ) << ( 3 + 8 ) ) | ( ( (Pixel >> 10) & 0x1f ) << ( 3 + 16 ) );
					*output_buf++ = Pixel & 0xffffff;
				}
			}
		}
	}
	
	// make this the current window we are drawing to
	//FrameBuffer_DebugWindow->OpenGL_MakeCurrentWindow ();
	
	//glPixelZoom ( (float)MainProgramWindow_Width / (float)DrawWidth, (float)MainProgramWindow_Height / (float)DrawHeight );
	//glDrawPixels ( FrameBuffer_Width, FrameBuffer_Height, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*) PixelBuffer );

	// update screen
	//FrameBuffer_DebugWindow->FlipScreen ();
	
	// this is no longer the current window we are drawing to
	//FrameBuffer_DebugWindow->OpenGL_ReleaseWindow ();
	
}


void GPU::Draw_FrameBuffers ()
{
	// frame buffer data
	FRAME_t *Frame = &GPURegsGp.FRAME_1;
	
	//swap for testing
	//Copy_Buffer ( & ( PixelBuffer [ 0 ] ), & ( RAM32 [ Frame [ 0 ].FBP << 11 ] ), c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, Frame [ 0 ].FBW << 6, 480, Frame [ 0 ].PSM );
	//Copy_Buffer ( & ( PixelBuffer [ 480 * c_iFrameBuffer_DisplayWidth ] ), & ( RAM32 [ Frame [ 1 ].FBP << 11 ] ), c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, Frame [ 1 ].FBW << 6, 480, Frame [ 1 ].PSM );
	Copy_Buffer ( & ( PixelBuffer [ 480 * c_iFrameBuffer_DisplayWidth ] ), & ( RAM32 [ Frame [ 0 ].FBP << 11 ] ), c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, Frame [ 0 ].FBW << 6, 480, Frame [ 0 ].PSM );
	Copy_Buffer ( & ( PixelBuffer [ 0 ] ), & ( RAM32 [ Frame [ 1 ].FBP << 11 ] ), c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, Frame [ 1 ].FBW << 6, 480, Frame [ 1 ].PSM );

	// testing
	//Copy_Buffer ( & ( PixelBuffer [ 0 ] ), & ( RAM32 [ 0x1a40 << 6 ] ), c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, 32, 256 );
	//Copy_Buffer ( & ( PixelBuffer [ 480 * c_iFrameBuffer_DisplayWidth ] ), & ( RAM32 [ 0x1b40 << 6 ] ), c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, 64, 64 );
	
	// make this the current window we are drawing to
	FrameBuffer_DebugWindow->OpenGL_MakeCurrentWindow ();
	
	//glPixelZoom ( (float)MainProgramWindow_Width / (float)DrawWidth, (float)MainProgramWindow_Height / (float)DrawHeight );
	glDrawPixels ( c_iFrameBuffer_DisplayWidth, c_iFrameBuffer_DisplayHeight, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*) PixelBuffer );

	// update screen
	FrameBuffer_DebugWindow->FlipScreen ();
	
	// this is no longer the current window we are drawing to
	FrameBuffer_DebugWindow->OpenGL_ReleaseWindow ();
}



void GPU::Copy_VRAM_toGPU ()
{
	u32 *pBuf32;
	u16 *pRAM16;
	int iIdx;
	int i;


	// make sure gpu is completely finished with anything
	//GPU::Finish ();


	DisplayOutput_Window->OpenGL_MakeCurrentWindow ();


	// make sure opengl is all synced up
	glFinish();


#ifdef ALLOW_OPENCL_PS2_TEST_CLUT
	// copy clut
	glBindBuffer ( GL_COPY_READ_BUFFER, ssbo_sclut );
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssbo_clut);
	//memcpy( p_uHwClutData32, _GPU->InternalCLUT, sizeof(_GPU->InternalCLUT) );
	for ( i = 0; i < 512; i++ )
	{
		p_uHwClutData32 [ i ] = ((u16*)_GPU->InternalCLUT)[ i ];
	}

	glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, sizeof(_GPU->InternalCLUT) * 2 );

	// copy cbpx
	glBindBuffer ( GL_COPY_READ_BUFFER, ssbo_scbpx );
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssbo_cbpx);

	memcpy( p_uHwCbpxData32, _GPU->CBPX, sizeof(_GPU->CBPX) );

	glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, sizeof(_GPU->CBPX) );
#endif


	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

	pBuf32 = (u32*) glMapBuffer ( GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY );

	
	//////////////////////////////////////////////////////
	// transfer pixel of image from VRAM
	for ( iIdx = 0; iIdx < (c_iRAM_Size >> 1); iIdx++ )
	{
		pBuf32 [ iIdx ] = (u32) RAM16 [ iIdx ];
	}

	

	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );


	DisplayOutput_Window->OpenGL_ReleaseWindow ();

}

void GPU::Copy_VRAM_toCPU ()
{
	u32 *pBuf32;
	int iIdx;


	// make sure gpu is completely finished with anything
	//GPU::Finish ();


	DisplayOutput_Window->OpenGL_MakeCurrentWindow ();

	// make sure opengl is all synced up
	glFinish();

	// copy out vram
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

	pBuf32 = (u32*) glMapBuffer ( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY );


	
	//////////////////////////////////////////////////////
	// transfer pixel of image from VRAM
	for ( iIdx = 0; iIdx < (c_iRAM_Size >> 1); iIdx++ )
	{
		RAM16 [ iIdx ] = (u16) pBuf32 [ iIdx ];
	}
	

	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );


#ifdef ALLOW_OPENCL_PS2_TEST_CLUT
	// copy out the clut
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssbo_clut);
	pBuf32 = (u32*) glMapBuffer ( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY );
	for ( iIdx = 0; iIdx < 512; iIdx++ )
	{
		InternalCLUT [ iIdx ] = (u16) pBuf32 [ iIdx ];
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );


	// copy out cbpx
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssbo_cbpx);

	pBuf32 = (u32*) glMapBuffer ( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY );

	CBPX[0] = pBuf32 [0];
	CBPX[1] = pBuf32 [1];
	

	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	DisplayOutput_Window->OpenGL_ReleaseWindow ();
#endif

}




void GPU::DMA_Read ( u32* Data, int ByteReadCount )
{
#ifdef INLINE_DEBUG_DMA_READ
	debug << "\r\n\r\nDMA_Read " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << "; Data = " << hex << Data [ 0 ];
#endif

}




void GPU::DMA_Write ( u32* Data, int ByteWriteCount )
{
#ifdef INLINE_DEBUG_DMA_WRITE
	debug << "\r\n\r\nDMA_Write " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << "; Data = " << hex << Data [ 0 ];
#endif

}



bool GPU::DMA_Write_Ready ()
{
#ifdef INLINE_DEBUG_DMA_WRITE_READY
	debug << "\r\nGPU::DMA_Write_Ready";
	debug << " M3R=" << _GPU->GIFRegs.STAT.M3R;
	debug << " M3P=" << _GPU->GIFRegs.STAT.M3P;
	debug << " XDIR=" << _GPU->GPURegsGp.TRXDIR.XDIR;
	debug << " DIR=" << _GPU->GIFRegs.STAT.DIR;
	debug << " P3E=" << _GPU->EndOfPacket [ 3 ];
	debug << " P3C=" << GPU::_GPU->ulTransferCount [ 3 ];
#endif

	if ( !GPU::_GPU->ulTransferCount [ 3 ] )
	{
		// check if data is getting transmitted in a different direction
		//if ( _GPU->GPURegsGp.TRXDIR.XDIR == 1 )
		//{
		//	return false;
		//}
		
		// check if buffer is going in opposite direction
		if ( _GPU->GIFRegs.STAT.DIR )
		{
			return false;
		}
		
		// if PSE is set, then all transfers are supposed to stop
		if ( _GPU->GIFRegs.STAT.PSE )
		{
			return false;
		}
	}
	
	// if path 3 is not currently in the middle of a transfer, then it is masked
	// check for the end of a packet
	if ( _GPU->EndOfPacket [ 3 ] )
	{
		// at the end of a packet, can mask path 3
		if ( _GPU->GIFRegs.STAT.M3R || _GPU->GIFRegs.STAT.M3P )
		{
#ifdef INLINE_DEBUG_DMA_WRITE_READY
	debug << " GPU: ALERT: Transfer via path3 while it is masked!!!";
	debug << " Path3Count=" << _GPU->ulTransferCount [ 3 ];
	debug << " IMT=" << _GPU->GIFRegs.MODE.IMT;
#endif

		// path 3 is masked //
		
			// for now need to act like data has been loaded into FIFO
			// even when path3 is masked, it must be loading it into FIFO and then just not processing it
			_GPU->GIFRegs.STAT.FQC = 16;
			
			// path3 in queue ???
			_GPU->GIFRegs.STAT.P3Q = 1;
			
			// no longer transferring via path 3
			if ( _GPU->GIFRegs.STAT.APATH == 3 )
			{
				_GPU->GIFRegs.STAT.APATH = 0;
			}
			
			return false;
		}
		
		
		// also at the end of a packet, can switch to path1 or path 2 if they are in queue
		if (
			( _GPU->GIFRegs.STAT.P1Q )
			
			// problem here is directhl could be in queue, which doesn't run at all if path 3 is in queue or running and isn't masked
			// || ( _GPU->GIFRegs.STAT.P2Q )
		)
		{
			// for now need to act like data has been loaded into FIFO
			// even when path3 is masked, it must be loading it into FIFO and then just not processing it
			_GPU->GIFRegs.STAT.FQC = 16;
			
			// path3 in queue ???
			_GPU->GIFRegs.STAT.P3Q = 1;

			// no longer transferring via path 3
			if ( _GPU->GIFRegs.STAT.APATH == 3 )
			{
				_GPU->GIFRegs.STAT.APATH = 0;
			}
			
			return false;
		}

	}
	
	
	// if currently transferring via path 1 or path 2, then must wait
	if ( /*( _GPU->GIFRegs.STAT.APATH == 1 ) ||*/ ( _GPU->GIFRegs.STAT.APATH == 2 ) )
	{
		_GPU->GIFRegs.STAT.FQC = 16;
		
		// path3 in queue ???
		_GPU->GIFRegs.STAT.P3Q = 1;
		
		return false;
	}
	
	
	/*
	// if path 2 is currently transferring, then can't transfer either
	//if ( !_GPU->EndOfPacket [ 2 ] )
	if (
			// check if GPU is currently transferring data
			( _GPU->ulTransferCount [ 2 ] )
			&&
			// while DMA#1 (VU1) is activated
			( Playstation2::Dma::pRegData [ 1 ]->CHCR.STR )
		)
		
	{
#ifdef INLINE_DEBUG_DMA_WRITE
	debug << " PATH2-BUSY PATH2-TRANSFER-COUNT=" << dec << _GPU->ulTransferCount [ 2 ];
#endif

		return false;
	}
	*/
	
	
	
	
	// path 3 NOT masked //
	return true;
}


u32 GPU::DMA_WriteBlock ( u64* Data, u32 QuadwordCount )
{
#ifdef INLINE_DEBUG_DMA_WRITE
	debug << "\r\n\r\nGPU::DMA_WriteBlock " << hex << setw ( 8 ) << *_DebugPC << " QWC=" << QuadwordCount << " " << dec << *_DebugCycleCount;
	debug << " P3C=" << GPU::_GPU->ulTransferCount [ 3 ];
	debug << " IMT=" << _GPU->GIFRegs.MODE.IMT;
	debug << hex << " DATA=";
	for ( int i = 0; i < ( QuadwordCount * 2 ); i++ ) debug << " " << Data [ i ];
	debug << "\r\n";
#endif

	int i;
	u32 DataTransferred, DataRemaining;
	u32 TransferTotal = 0;

	// check if path 3 is masked
	if ( _GPU->GIFRegs.STAT.M3R || _GPU->GIFRegs.STAT.M3P )
	{
		// *** testing ***
		//return 0;

		if ( _GPU->EndOfPacket [ 3 ] )
		{

		// display warning for now
#ifdef VERBOSE_PATH3MASK
		cout << "\nhps2x64: GPU: ALERT: Transfer via path 3 while it is masked!!!";
#endif

#ifdef INLINE_DEBUG_DMA_WRITE
	debug << "\r\nhps2x64: GPU: ALERT: Transfer via path 3 while it is masked!!!";
#endif

		}

	}

#ifdef ENABLE_PATH_PROGRESS_ALERT
	// check if transfering while path 1 or path 2 packet is in progress
	if ( !_GPU->EndOfPacket [ 2 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 3 TRANSFER WHILE GPU PATH 2 PACKET TRANSFER IN PROGRESS!";
	}
	if ( !_GPU->EndOfPacket [ 1 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 3 TRANSFER WHILE GPU PATH 1 PACKET TRANSFER IN PROGRESS!";
	}
#endif


#ifdef ENABLE_PATH1_BUFFER
	// check if path 1 is in queue
	/*
	if ( _GPU->ullP1Buf_ReadIndex != _GPU->ullP1Buf_WriteIndex )
	{
		if ( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) ] <= *_DebugCycleCount )
		{
			// path 1 is in queue
			_GPU->GIFRegs.STAT.P1Q = 1;
			
			// check if path 1 can transfer (in between path 2 packets or path 2 not started)
			if (
					( !_GPU->GIFRegs.STAT.APATH )
				||	( _GPU->EndOfPacket [ 3 ] )
				||	( !_GPU->ulTransferCount [ 3 ] )
				||	( _GPU->GIFRegs.MODE.IMT )
			)
			{
				// this will also clear P1Q if needed
				GPU::Path1_ProcessBlocks ();
				
			}
		}
	}
	*/
#endif


	if ( _GPU->CurrentPath != 3 )
	{
#ifdef INLINE_DEBUG_DMA_WRITE
	debug << " VERTEX-COUNT-RESET";
#endif

		if ( !GPU::ulThreadedGPU )
		{
			_GPU->lVertexCount = 0;
		}
		else
		{
			// reset vertex count on the gpu thread when multi-threading
			lVertexFlag = 1;
		}
	}
	
	
	// this is path 3
	_GPU->GIFRegs.STAT.APATH = 3;
	
	// path 3 no longer in queue
	_GPU->GIFRegs.STAT.P3Q = 0;
	
	// path 3 no longer interrupted
	_GPU->GIFRegs.STAT.IP3 = 0;
	
	// set amount of data loaded into buffer
	_GPU->GIFRegs.STAT.FQC = QuadwordCount;
	
	// data is going into the GPU
	//_GPU->GIFRegs.STAT.DIR = 0;
	_GPU->GIFRegs.STAT.OPH = 0;
	
	
	// this would be path 3
	_GPU->CurrentPath = 3;
	
	// set fifo size
	_GPU->FifoSize = QuadwordCount;
	
	// set GPU as busy for 32 cycles for now
	//_GPU->BusyUntil_Cycle = *_DebugCycleCount + 32;
	_GPU->BusyUntil_Cycle = *_DebugCycleCount + QuadwordCount;
	
	// have not reached end of packet yet
	_GPU->EndOfPacket [ 3 ] = 0;

	while ( QuadwordCount )
	{
#ifdef INLINE_DEBUG_DMA_WRITE
	debug << "\r\n";
	debug << hex << Data [ 0 ] << " " << Data [ 1 ];
#endif

		// check if in-between transferring blocks
		//if ( !_GPU->ulTransferCount [ 3 ] )
		if ( _GPU->EndOfPacket [ 3 ] )
		{
			// note: should always break at the end of a packet for arbitration ??
			//if ( _GPU->GIFRegs.STAT.M3R || _GPU->GIFRegs.STAT.M3P )
			//{
				// break in between transfers for now if path3 is masked
				break;
			//}
		}
		
		
		DataRemaining = _GPU->GIF_FIFO_Execute ( Data, QuadwordCount << 1 );
		
		DataTransferred = ( QuadwordCount << 1 ) - DataRemaining;
		TransferTotal += ( DataTransferred >> 1 );
		
		// update pointer with number of 64-bit elements to advance
		//Data += 2;
		Data += DataTransferred;
		
		// get the number of quadwords remaining from 64-bit elements remaining
		QuadwordCount = DataRemaining >> 1;
		
	}	// end while ( QuadwordCount )

	
	//if ( !_GPU->ulTransferCount [ 3 ] )
	if ( _GPU->EndOfPacket [ 3 ] )
	{
		//if ( _GPU->GIFRegs.STAT.M3R || _GPU->GIFRegs.STAT.M3P )
		if ( !VU::_VU[ 1 ]->VifRegs.STAT.VIS )
		{
		// for now, should disable stop on vif to check if it should continue
		VU::_VU[ 1 ]->VifStopped = 0;
		
		// restart dma#1
		//Dma::_DMA->Transfer ( 1 );
		}
	}

	
	// check if data is now going the other way
	if ( _GPU->GPURegsGp.TRXDIR.XDIR == 1 )
	{
		// if data is going the other way, then buffer should be clear
		_GPU->GIFRegs.STAT.FQC = 0;
	}
	
	
	// for now, trigger signals
	// ***TODO*** add correct event triggering
	//_GPU->GPURegs1.CSR.Value |= 0xf;
	
	// for now: check for GPU interrupt
	//if ( _GPU->GPURegs1.CSR.Value & ( _GPU->GPURegs1.IMR >> 8 ) )
	//{
//#ifdef INLINE_DEBUG_DMA_WRITE
//	debug << "; GSINT";
//#endif
	//
	//	SetInterrupt ();
	//}
	
	// return the amount of data written
	//return QuadwordCount;
	return TransferTotal;
}


/*
u32 GPU::DMA_ReadBlock ( u64* Data, u32 QuadwordCount )
{
#ifdef INLINE_DEBUG_DMA_READ
	debug << "\r\n\r\nDMA_ReadBlock " << hex << setw ( 8 ) << *_DebugPC << " QWC=" << QuadwordCount << " " << dec << *_DebugCycleCount << hex << "; Data= ";
	debug << "\r\n";
#endif

	// this would be path 3
	_GPU->CurrentPath = 3;
	
	// set fifo size
	_GPU->FifoSize = QuadwordCount;
	
	// set GPU as busy for 32 cycles for now
	_GPU->BusyUntil_Cycle = *_DebugCycleCount + 32;

	// read data from GPU and specify amount of data to read in words
	_GPU->TransferDataOut32 ( (u32*) Data, QuadwordCount << 2 );
	
	// return the amount of data written
	return QuadwordCount;
}
*/

// xgkick
#ifdef OLD_PATH1_ARGS
void GPU::Path1_WriteBlock ( u64* Data )
#else
void GPU::Path1_WriteBlock ( u64* pMemory64, u32 Address, u32 Count )
#endif
{
#ifdef INLINE_DEBUG_PATH1_WRITE
	debug << "\r\n\r\nPATH1_WriteBlock " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << hex << "; Data= ";
#ifdef OLD_PATH1_ARGS
	debug << " " << Data [ 0 ] << " " << Data [ 1 ];
#else
	debug << hex << pMemory64 [ ( ( Address & 0x3ff ) << 1 ) + 0 ] << " " << pMemory64 [ ( ( Address & 0x3ff ) << 1 ) + 1 ];
	debug << " ADDR=" << Address;
#endif
#endif

	// check if transfering while path 3 or path 2 packet is in progress
#ifdef ENABLE_PATH_PROGRESS_ALERT
	if ( !_GPU->EndOfPacket [ 3 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 1 TRANSFER WHILE GPU PATH 3 PACKET TRANSFER IN PROGRESS! IMT=" << _GPU->GIFRegs.MODE.IMT;
	}
	if ( !_GPU->EndOfPacket [ 2 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 1 TRANSFER WHILE GPU PATH 2 PACKET TRANSFER IN PROGRESS!";
	}
#endif
	
	if ( _GPU->CurrentPath != 1 )
	{
#ifdef INLINE_DEBUG_PATH1_WRITE
	debug << " VERTEX-COUNT-RESET";
#endif

		if ( !GPU::ulThreadedGPU )
		{
			_GPU->lVertexCount = 0;
		}
		else
		{
			// reset vertex count on the gpu thread when multi-threading
			lVertexFlag = 1;
		}
	}

	// for path1, need to keep reading data until end of packet reached
	_GPU->CurrentPath = 1;
	
	// if device not busy, then clear fifo size
	if ( *_DebugCycleCount < _GPU->BusyUntil_Cycle )
	{
		_GPU->FifoSize = 0;
	}
	else
	{
		// otherwise, add the data into fifo since device is busy
		_GPU->FifoSize++;
	}
	
	// path1 is not done since it is just starting
	_GPU->Tag_Done = 0;
	_GPU->EndOfPacket [ 1 ] = 0;
	
#ifdef DISABLE_PATH1_WRAP
	// mask address
	Address &= 0x3ff;
#endif

	// loop while path1 is not done
	//while ( !_GPU->Tag_Done )
	while ( !_GPU->EndOfPacket [ 1 ]
#ifdef DISABLE_PATH1_WRAP
	// don't let the address wrap around VU1 data memory
	&& ( Address < 0x400 )
#endif
	// should be able to send path 1 data over time instead of all at once
	&& ( Count > 0 )
	)
	{
#ifdef INLINE_DEBUG_PATH1_WRITE
	debug << "\r\n";
#ifdef OLD_PATH1_ARGS
	debug << hex << Data [ 0 ] << " " << Data [ 1 ];
#else
	debug << hex << "ADDR:" << Address << " -> " << pMemory64 [ ( ( Address & 0x3ff ) << 1 ) + 0 ] << " " << pMemory64 [ ( ( Address & 0x3ff ) << 1 ) + 1 ];
#endif
#endif

#ifdef OLD_PATH1_ARGS
		_GPU->GIF_FIFO_Execute ( Data [ 0 ], Data [ 1 ] );
		
		Data += 2;
#else
	
		_GPU->GIF_FIFO_Execute ( & ( pMemory64 [ ( ( Address & 0x3ff ) << 1 ) + 0 ] ), 2 );
		
		Address += 1;
#endif

		// only transfer specified amount of data via path 1
		Count -= 1;
	}
	
#ifdef DISABLE_PATH1_WRAP
	if ( Address > 0x3ff )
	{
#ifdef VERBOSE_PATH1_END_OF_MEMORY
		cout << "\nhps2x64: INFO: ***VU PATH1 - WENT PAST END OF MEMORY***\n";
#endif
	}
#endif
	
	// testing INT
	//SetInterrupt ();
}



void GPU::Path1_WriteBlock3 ()
{
#ifdef INLINE_DEBUG_PATH1_WRITE
	debug << "\r\n\r\nPATH1_WriteBlock3 " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << hex << "; Data= ";
	debug << hex << ullPath1Buffer [ ( ( ( ullP1Buf_ReadIndex & c_iPath1BufMask ) << 1 ) ) + 0 ] << " " << ullPath1Buffer [ ( ( ( ullP1Buf_ReadIndex & c_iPath1BufMask ) << 1 ) ) + 1 ];
#endif

	u64 *pBuf;
	u64 ullCount128;

	// check if transfering while path 3 or path 2 packet is in progress
#ifdef ENABLE_PATH_PROGRESS_ALERT
	if ( !_GPU->EndOfPacket [ 3 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 1 TRANSFER WHILE GPU PATH 3 PACKET TRANSFER IN PROGRESS! IMT=" << _GPU->GIFRegs.MODE.IMT;
	}
	if ( !_GPU->EndOfPacket [ 2 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 1 TRANSFER WHILE GPU PATH 2 PACKET TRANSFER IN PROGRESS!";
	}
#endif
	
	if ( _GPU->CurrentPath != 1 )
	{
#ifdef INLINE_DEBUG_PATH1_WRITE
	debug << " VERTEX-COUNT-RESET";
#endif

		if ( !GPU::ulThreadedGPU )
		{
			_GPU->lVertexCount = 0;
		}
		else
		{
			// reset vertex count on the gpu thread when multi-threading
			lVertexFlag = 1;
		}
	}

	// for path1, need to keep reading data until end of packet reached
	_GPU->CurrentPath = 1;
	
	// path1 is not done since it is just starting
	_GPU->Tag_Done = 0;
	_GPU->EndOfPacket [ 1 ] = 0;
	

	while ( _GPU->ullP1Buf_ReadIndex < _GPU->ullP1Buf_WriteIndex )
	{
		// get the count of items
		pBuf = (u64*) & _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & c_iPath1BufMask ) << 1 ) ];
		_GPU->ullP1Buf_ReadIndex++;

		ullCount128 = pBuf [ 1 ];

		if ( ullCount128 )
		{
			for ( int i = 0; i < ullCount128; i++ )
			{
#ifdef INLINE_DEBUG_PATH1_WRITE
	debug << "\r\n";
	debug << hex << ullPath1Buffer [ ( ( ( ullP1Buf_ReadIndex & c_iPath1BufMask ) << 1 ) ) + 0 ] << " " << ullPath1Buffer [ ( ( ( ullP1Buf_ReadIndex & c_iPath1BufMask ) << 1 ) ) + 1 ];
#endif

				_GPU->GIF_FIFO_Execute ( (u64*) & ( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & c_iPath1BufMask ) << 1 ) ] ), 2 );
				
				_GPU->ullP1Buf_ReadIndex++;
			}
		}
	}
	
}





void GPU::Path1_WriteBlock2 ()
{
#ifdef INLINE_DEBUG_PATH1_WRITE2
	debug << "\r\n\r\nPATH1_WriteBlock2 " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << hex << "; Data= ";
#endif

	int iCount;
	
	// for now, just write path 1 unconditionally
	// don't write path 1 if path 2 in progress or imt not set on path 3
	if ( ( GPU::_GPU->GIFRegs.STAT.APATH == 2 )
		|| ( ( GPU::_GPU->GIFRegs.STAT.APATH == 3 ) && !( GPU::_GPU->GIFRegs.STAT.IMT ) )
	)
	{
		// another path is running and should not be interrupted
		return;
	}
	
	// check if there is data to write
	if ( _GPU->ullP1Buf_ReadIndex == _GPU->ullP1Buf_WriteIndex )
	{
		// there is no data to write into gpu from path 1
		return;
	}
		
	
	if ( _GPU->CurrentPath != 1 )
	{
#ifdef INLINE_DEBUG_PATH1_WRITE2
	debug << " VERTEX-COUNT-RESET";
#endif

		if ( !GPU::ulThreadedGPU )
		{
			_GPU->lVertexCount = 0;
		}
		else
		{
			// reset vertex count on the gpu thread when multi-threading
			lVertexFlag = 1;
		}
	}

	// for path1, need to keep reading data until end of packet reached
	_GPU->CurrentPath = 1;
	
	// if device not busy, then clear fifo size
	/*
	if ( *_DebugCycleCount < _GPU->BusyUntil_Cycle )
	{
		_GPU->FifoSize = 0;
	}
	else
	{
		// otherwise, add the data into fifo since device is busy
		_GPU->FifoSize++;
	}
	*/
	
	// path1 is not done since it is just starting
	_GPU->Tag_Done = 0;
	_GPU->EndOfPacket [ 1 ] = 0;
	
	
	// clear the path 1 count before writing block
	GPU::_GPU->ulTransferCount [ 1 ] = 0;

	
	// get the count of data to transfer
	iCount = _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) + 1 ];
	_GPU->ullP1Buf_ReadIndex++;
	
	
	for ( int iIdx = 0; iIdx < iCount; iIdx++ )
	{
		_GPU->GIF_FIFO_Execute ( (u64*) & ( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) + 0 ] ), 2 );
		_GPU->ullP1Buf_ReadIndex++;
	}
	

	// check if path1 still in queue
	if (
		( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) ] > *_DebugCycleCount )
		||	( _GPU->ullP1Buf_ReadIndex == _GPU->ullP1Buf_WriteIndex )
	)
	{
		// path 1 no longer in queue
		_GPU->GIFRegs.STAT.P1Q = 0;
	}
}

// this will just flush the blocks if there is no other path in the way
/*
static void GPU::Path1_FlushBlocks ()
{
	// for now, just write path 1 unconditionally
	// don't write path 1 if path 2 in progress or imt not set on path 3
	if ( ( GPU::_GPU->GIFRegs.STAT.APATH == 2 )
		|| ( ( GPU::_GPU->GIFRegs.STAT.APATH == 3 ) && !( GPU::_GPU->GIFRegs.STAT.IMT ) )
	)
	{
		// another path is running and should not be interrupted
		return;
	}
	
	// check if there is data to write
	while ( _GPU->ullP1Buf_ReadIndex != _GPU->ullP1Buf_WriteIndex )
	{
		// there is no data to write into gpu from path 1
		GPU::Path1_WriteBlock2 ();
	}
}
*/


// this will flush the blocks if there is no other path in the way, but additionally checks the cycle#
void GPU::Path1_ProcessBlocks ()
{
#ifdef INLINE_DEBUG_PROCESSBLOCKS
	debug << "\r\n\r\nPath1_ProcessBlocks " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << hex << "; Data= ";
	debug << " WI=" << dec << _GPU->ullP1Buf_WriteIndex;
	debug << " RI(before)=" << dec << _GPU->ullP1Buf_ReadIndex;
	debug << " APATH=" << GPU::_GPU->GIFRegs.STAT.APATH;
#endif

	// for now, if there is data then write it into the stream or process it
	if ( _GPU->ullP1Buf_ReadIndex != _GPU->ullP1Buf_WriteIndex )
	{
		Path1_WriteBlock3 ();
	}

	/*
	if ( _GPU->ullP1Buf_ReadIndex != _GPU->ullP1Buf_WriteIndex
			&& ( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) ] <= *_DebugCycleCount )
			)
	{
#ifdef INLINE_DEBUG_PROCESSBLOCKS
	debug << " IN-QUEUE";
#endif

		// path 1 is in queue
		_GPU->GIFRegs.STAT.P1Q = 1;
		
		// for now, just write path 1 unconditionally
		// don't write path 1 if path 2 in progress or imt not set on path 3
		if ( ( ( GPU::_GPU->GIFRegs.STAT.APATH == 2 ) && ( _GPU->ulTransferCount [ 2 ] ) )
			|| ( ( GPU::_GPU->GIFRegs.STAT.APATH == 3 ) && ( _GPU->ulTransferCount [ 3 ] ) && !( GPU::_GPU->GIFRegs.STAT.IMT ) )
		)
		{
#ifdef INLINE_DEBUG_PROCESSBLOCKS
	debug << " PATH-IN-USE";
#endif

			// another path is running and should not be interrupted
			return;
		}
		
		// check if there is data to write
		while ( _GPU->ullP1Buf_ReadIndex != _GPU->ullP1Buf_WriteIndex
				&& ( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) ] <= *_DebugCycleCount )
				)
		{
			// there is data to write into gpu from path 1
			GPU::Path1_WriteBlock2 ();
		}
	}
	*/
	
#ifdef INLINE_DEBUG_PROCESSBLOCKS
	debug << " RI(after)=" << dec << _GPU->ullP1Buf_ReadIndex;
#endif
}


// vif1
// due to issues with possible unalignments, etc (thought it wasn't supposed to be possible on PS2??), must count units in 64-bit words
//static void GPU::Path2_WriteBlock ( u64* Data, u32 QuadwordCount )
//static void GPU::Path2_WriteBlock ( u64* Data, u32 DoublewordCount )
void GPU::Path2_WriteBlock ( u32* Data, u32 WordCount )
{
#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << "\r\n\r\nPATH2_WriteBlock " << hex << setw ( 8 ) << *_DebugPC << " WC=" << WordCount << " " << dec << *_DebugCycleCount << hex << "; Data= ";
	debug << " PathCount(before)=" << dec << _GPU->ulTransferCount [ 2 ];
	//debug << " LSA=" << hex << Playstation1::SPU2::_SPU2->SPU0.pCoreRegs0->ChRegs1 [ 0 ].LSA;
	for ( int i = 0; i < WordCount; i++ ) debug << " " << hex << Data [ i ];
	debug << "\r\n";
#endif

	u32 QuadwordCount;
	u32 DataTransferred, DataRemaining;
	
	// if there is no data to transfer, return immediately
	//if ( !DoublewordCount )
	if ( !WordCount )
	{
#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << " NO-DATA";
#endif

		// no data ??
		return;
	}

	
#ifdef ENABLE_PATH_PROGRESS_ALERT
	// check if transfering while path 1 or path 3 packet is in progress
	if ( !_GPU->EndOfPacket [ 3 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 2 TRANSFER WHILE GPU PATH 3 PACKET TRANSFER IN PROGRESS! IMT=" << _GPU->GIFRegs.MODE.IMT;
	}
	if ( !_GPU->EndOfPacket [ 1 ] )
	{
		cout << "\nhps2x64: ALERT: PATH 2 TRANSFER WHILE GPU PATH 1 PACKET TRANSFER IN PROGRESS!";
	}
#endif


#ifdef ENABLE_PATH1_BUFFER
	// check if path 1 is in queue
	/*
	if ( _GPU->ullP1Buf_ReadIndex != _GPU->ullP1Buf_WriteIndex )
	{
		if ( _GPU->ullPath1Buffer [ ( ( _GPU->ullP1Buf_ReadIndex & GPU::c_iPath1BufMask ) << 1 ) ] <= *_DebugCycleCount )
		{
			// path 1 is in queue
			_GPU->GIFRegs.STAT.P1Q = 1;
			
			// check if path 1 can transfer (in between path 2 packets or path 2 not started)
			// it sets APATH=2 before calling here, so no need to check that, but checking anyway
			if (
					( !_GPU->GIFRegs.STAT.APATH )
				||	( _GPU->EndOfPacket [ 2 ] )
				||	( !_GPU->ulTransferCount [ 2 ] )
			)
			{
				// this will also clear P1Q if needed
				GPU::Path1_ProcessBlocks ();
				
			}
		}
	}
	*/
#endif


	if ( _GPU->CurrentPath != 2 )
	{
#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << " VERTEX-COUNT-RESET";
#endif

		if ( !GPU::ulThreadedGPU )
		{
			_GPU->lVertexCount = 0;
		}
		else
		{
			// reset vertex count on the gpu thread when multi-threading
			lVertexFlag = 1;
		}
	}
	
	
	_GPU->CurrentPath = 2;

	_GPU->EndOfPacket [ 2 ]	= 0;
	
	// if device not busy, then clear fifo size
	/*
	if ( *_DebugCycleCount < _GPU->BusyUntil_Cycle )
	{
		_GPU->FifoSize = 0;
	}
	else
	{
		// otherwise, add the data into fifo since device is busy
		_GPU->FifoSize++;
	}
	*/
	
	// set GPU as busy for 32 cycles for now
	//_GPU->BusyUntil_Cycle = *_DebugCycleCount + 32;
	_GPU->BusyUntil_Cycle = *_DebugCycleCount + ( ( WordCount + 3 ) >> 2 );
	
	
	// check if there is an extra piece of 64-bit data to transfer
	if ( _GPU->ulPath2_DataWaiting && WordCount )
	{
#ifdef INLINE_DEBUG_PATH2_WRITE
	//debug << "\r\n";
	//debug << hex << _GPU->ullPath2_Data << " " << Data [ 0 ];
#endif

		// need to have at least a quadword of data to send
		for ( int i = _GPU->ulPath2_DataWaiting; i < 4; i++ )
		{
			_GPU->ullPath2_Data [ i ] = *Data++;
		}

#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << "\r\n";
	debug << hex << ((u64*)_GPU->ullPath2_Data) [ 0 ] << " " << ((u64*)_GPU->ullPath2_Data) [ 1 ];
#endif

		// transfer the waiting data with the new data //
		
		_GPU->GIF_FIFO_Execute ( ((u64*)_GPU->ullPath2_Data), 2 );
		

		// have to send data in quadword chunks, so 1 qw=4 32-bit words
		//WordCount -= _GPU->ulPath2_DataWaiting;
		WordCount -= ( 4 - _GPU->ulPath2_DataWaiting );


		_GPU->ulPath2_DataWaiting = 0;
		//Data += 1;
		//DoublewordCount--;
	}
	
	// get the number of 128-bit pieces of data to transfer
	//QuadwordCount = DoublewordCount >> 1;
	QuadwordCount = WordCount >> 2;
	
	// its possible it could transfer less than a quad-word
	if ( QuadwordCount )
	{
#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << " QuadwordCount=" << dec << QuadwordCount;
#endif


		while ( QuadwordCount )
		{
#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << "\r\n";
	debug << hex << ((u64*)Data) [ 0 ] << " " << ((u64*)Data) [ 1 ];
#endif

			DataRemaining = _GPU->GIF_FIFO_Execute ( ((u64*)Data), QuadwordCount << 1 );
			
			// get the amount transferred
			DataTransferred = ( QuadwordCount << 1 ) - DataRemaining;
			
			// update qwc count so it counts quadwords remaining
			QuadwordCount = DataRemaining >> 1;
			
			//Data += 4;
			Data += ( DataTransferred << 1 );
			

#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << " DataRemaining=" << dec << DataRemaining;
	debug << " DataTransferred=" << dec << DataTransferred;
	debug << " PathCount=" << dec << _GPU->ulTransferCount [ 2 ];
#endif
			
		}	// end while ( QuadwordCount )
	}
	
	// check if there is an extra piece of 64-bit data on the end
	// turns out need to check for data in 32-bit units
	//_GPU->ulPath2_DataWaiting = DoublewordCount & 1;
	_GPU->ulPath2_DataWaiting = WordCount & 3;
	
	// if so, then transfer it next time if possible
	if ( _GPU->ulPath2_DataWaiting )
	{
		for ( int i = 0; i < _GPU->ulPath2_DataWaiting; i++ )
		{
			_GPU->ullPath2_Data [ i ] = Data [ i ];
		}
	}

	// testing INT
	//SetInterrupt ();
	
#ifdef INLINE_DEBUG_PATH2_WRITE
	debug << " PathCount(after)=" << dec << _GPU->ulTransferCount [ 2 ];
#endif

}


// vif1
void GPU::Path2_ReadBlock ( u64* Data, u32 QuadwordCount )
{
	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	/*
	if ( VU::ulThreadCount )
	{

		// wait for vu thread to finish
		VU::Finish ();

		// wait for GPU to finish
		Finish ();

	}
	else if ( _GPU->ulNumberOfThreads )
	{
		// flush gpu
		Finish ();
	}
	*/

	//if ( _GPU->ulNumberOfThreads )
	//{
	//	// for now, wait to finish
	//	//while ( _GPU->ulInputBuffer_Count );
	//	//Finish ();
	//}

#ifdef VERBOSE_READBLOCK
	cout << "\nhps2x64: GPU: Path2_ReadBlock\n";
#endif
	
	// this would be path 2
	_GPU->CurrentPath = 2;

	// set fifo size
	_GPU->FifoSize = QuadwordCount;

	
	// path 2
	_GPU->GIFRegs.STAT.APATH = 2;
	
	// data is being output, so set OPH in STAT
	_GPU->GIFRegs.STAT.OPH = 1;
	
	
	// set GPU as busy for 32 cycles for now
	//_GPU->BusyUntil_Cycle = *_DebugCycleCount + 32;
	_GPU->BusyUntil_Cycle = *_DebugCycleCount + QuadwordCount;

	// read data from GPU and specify amount of data to read in words
	_GPU->TransferDataOut32_DS ( (u32*) Data, QuadwordCount << 2 );
	
	// return the amount of data written
	//return QuadwordCount;
}



void GPU::SetDrawVars ( u64 *inputdata_ptr, u32 Coord0, u32 Coord1, u32 Coord2 )
{
#if defined INLINE_DEBUG_SETDRAWVARS || defined INLINE_DEBUG_PRIMITIVE
	debug << "; SetDrawVars";
#endif

	u64 ullPRIM;
	//u64 *inputdata_ptr;
	//u32 Coord0, Coord1, Coord2;

	// scissor data
	SCISSOR_t *Scissor = &GPURegsGp.SCISSOR_1;
	
	// frame buffer data
	FRAME_t *Frame = &GPURegsGp.FRAME_1;
	
	// z-buffer data
	ZBUF_t *ZBuf = &GPURegsGp.ZBUF_1;
	
	// maybe these should be pointers and static
	pTest = &GPURegsGp.TEST_1;
	
	// xy offset
	XYOFFSET_t *Offset = &GPURegsGp.XYOFFSET_1;
	
	CLAMP_t *pClamp = &GPURegsGp.CLAMP_1;
	
	// alpha correction value
	u64 *pFBA = &GPURegsGp.FBA_1;
	
	// get alpha selection
	ALPHA_t *pAlpha = &GPURegsGp.ALPHA_1;

	// need the texture data
	TEX0_t *pTEX0 = TEXX;
	TEX1_t *pTEX1 = &GPURegsGp.TEX1_1;

	
	// get context
	if ( GPURegsGp.PRMODECONT & 1 )
	{
		// attributes in PRIM //
		Ctx = GPURegsGp.PRIM.CTXT;
		TextureMapped = GPURegsGp.PRIM.TME;
		Gradient = GPURegsGp.PRIM.IIP;
		
		// anti-aliasing also enables alpha-blending ??
		//Alpha = GPURegsGp.PRIM.ABE;
		Alpha = GPURegsGp.PRIM.ABE | GPURegsGp.PRIM.AA1;
		
		Fst = GPURegsGp.PRIM.FST;
		FogEnable = GPURegsGp.PRIM.FGE;

		ullPRIM  = GPURegsGp.PRIM.Value & 0x7ff;
	}
	else
	{
		// attributes in PRMODE //
		Ctx = GPURegsGp.PRMODE.CTXT;
		TextureMapped = GPURegsGp.PRMODE.TME;
		Gradient = GPURegsGp.PRMODE.IIP;
		
		// anti-aliasing also enables alpha-blending ??
		//Alpha = GPURegsGp.PRMODE.ABE;
		Alpha = GPURegsGp.PRMODE.ABE | GPURegsGp.PRMODE.AA1;
		
		Fst = GPURegsGp.PRMODE.FST;
		FogEnable = GPURegsGp.PRMODE.FGE;

		ullPRIM  = ( ( GPURegsGp.PRIM.Value & 7 ) | ( GPURegsGp.PRMODE.Value & ~7 ) ) & 0x7ff;
	}
	
#if defined INLINE_DEBUG_SETDRAWVARS || defined INLINE_DEBUG_PRIMITIVE
	debug << " Context#" << Ctx << " Texture=" << TextureMapped;
#endif

	// get window for context
	Window_XLeft = Scissor [ Ctx ].SCAX0;
	Window_XRight = Scissor [ Ctx ].SCAX1;
	Window_YTop = Scissor [ Ctx ].SCAY0;
	Window_YBottom = Scissor [ Ctx ].SCAY1;
	
#if defined INLINE_DEBUG_SETDRAWVARS || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << " Window: XLeft=" << Window_XLeft << " XRight=" << Window_XRight << " YTop=" << Window_YTop << " YBottom=" << Window_YBottom;
#endif

	// get x,y offset for context (12.4 fixed point format)
	Coord_OffsetX = Offset [ Ctx ].OFX;
	Coord_OffsetY = Offset [ Ctx ].OFY;
	
#if defined INLINE_DEBUG_SETDRAWVARS || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << " Offset: OFX=" << dec << ( Coord_OffsetX >> 4 ) << " OFY=" << ( Coord_OffsetY >> 4 );
	//debug << dec << " (hex)Offset: OFX=" << hex << Coord_OffsetX << " OFY=" << Coord_OffsetY;
#endif

	// get frame buffer info for context
	FrameBufferStartOffset32 = Frame [ Ctx ].FBP << 11;
	FrameBufferWidth_Pixels = Frame [ Ctx ].FBW << 6;
	
	// actually, the FBW frame buffer width value is more important here (in units of 64 pixels)
	//FrameBufferWidth64 = Frame [ Ctx ].FBW;
	
	FrameBuffer_PixelFormat = Frame [ Ctx ].PSM;

	ZBUF_X.Value = ZBuf [ Ctx ].Value;
	ZBufferStartOffset32 = ZBUF_X.ZBP << 11;
	ZBuffer_PixelFormat = ZBUF_X.PSM;
	
	// clamp
	pClamp = & ( pClamp [ Ctx ] );
	Clamp_ModeX = pClamp->WMS;
	Clamp_ModeY = pClamp->WMT;
	Clamp_MinU = pClamp->MINU;
	Clamp_MaxU = pClamp->MAXU;
	Clamp_MinV = pClamp->MINV;
	Clamp_MaxV = pClamp->MAXV;
	
	// get alpha correction value
	FBA_X = pFBA [ Ctx ];
	
	// get alpha selection value
	ALPHA_X.Value = pAlpha [ Ctx ].Value;
	
	// get test value
	TEST_X.Value = pTest [ Ctx ].Value;
	
	pTest = & ( pTest [ Ctx ] );


	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	FBA_X &= 1;
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;


	// set alpha and destination alpha test for 24-bit frame buffer and NON-24-bit frame buffer //
	if ( ( Frame [ Ctx ].PSM & 3 ) == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
		
		// set destination alpha vars, enable all if 24-bit pixels
		DA_Enable = 0;
		
		// should only write 24-bits to frame buffer only?
		FrameBuffer_WriteMask32 = 0xffffff & ~Frame [ Ctx ].FBMSK;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
		
		// set destination alpha vars, enable all if 24-bit pixels
		DA_Enable = TEST_X.DATE;
		
		// probably should be the opposite
		DA_Test = TEST_X.DATM;
		//DA_Test = TEST_X.DATM ^ 1;
		
		DA_Enable |= ( DA_Enable << 15 );
		DA_Enable |= ( DA_Enable << 31 );
		
		DA_Test |= ( DA_Test << 15 );
		DA_Test |= ( DA_Test << 31 );
		
		// should write full 32-bits to frame buffer?
		FrameBuffer_WriteMask32 = 0xffffffff & ~Frame [ Ctx ].FBMSK;
		FrameBuffer_WriteMask16 = ( ( FrameBuffer_WriteMask32 >> 3 ) & 0x1f ) | ( ( FrameBuffer_WriteMask32 >> 6 ) & 0x3e0 ) | ( ( FrameBuffer_WriteMask32 >> 9 ) & 0x7c00 ) | ( ( FrameBuffer_WriteMask32 >> 16 ) & 0x8000 );
	}
	


	// set what to do if source alpha test fails
	Alpha_Fail = TEST_X.AFAIL;

	// initialize source alpha test vars //
	switch ( TEST_X.ATE )
	{
		case 0:
			SrcAlphaTest_Pass = 1;
			break;
			
		case 1:
		
			switch ( TEST_X.ATST )
			{
				// NEVER
				case 0:
					SrcAlphaTest_Pass = 0;
					break;
					
				// ALWAYS
				case 1:
					SrcAlphaTest_Pass = 1;
					break;
					
				// LESS
				case 2:
					// bottom 24 bits of SrcAlpha_ARef need to be cleared out first
					SrcAlpha_ARef = ( ( (u32) TEST_X.AREF ) << 24 );
					break;
					
				// LESS OR EQUAL
				case 3:
					// bottom 24 bits of SrcAlpha_ARef need to be set first
					SrcAlpha_ARef = ( ( (u32) TEST_X.AREF ) << 24 ) | 0xffffff;
					break;
					
				// EQUAL
				case 4:
					SrcAlpha_ARef = ( (u32) TEST_X.AREF );
					break;
					
				// GREATER OR EQUAL
				case 5:
					// bottom 24 bits of SrcAlpha_ARef need to be cleared out first
					SrcAlpha_ARef = ( ( (u32) TEST_X.AREF ) << 24 );
					break;
					
				// GREATER
				case 6:
					// bottom 24 bits of SrcAlpha_ARef need to be set first
					SrcAlpha_ARef = ( ( (u32) TEST_X.AREF ) << 24 ) | 0xffffff;
					break;
					
				// NOT EQUAL
				case 7:
					SrcAlpha_ARef = ( (u32) TEST_X.AREF );
					break;
			}
			
			break;
	}


	// initialize offset for z-buffer test //
	// ***TODO*** can remove this switch statement
	switch ( TEST_X.ZTST )
	{
		// NEVER pass
		case 0:
			DepthTest_Offset = -0x8000000000000000LL;
			break;
			
		// ALWAYS pass
		case 1:
			DepthTest_Offset = 0x100000000LL;
			break;
			
		// GREATER OR EQUAL pass
		case 2:
			DepthTest_Offset = 1;
			break;
			
		// GREATER
		case 3:
			DepthTest_Offset = 0;
			break;
	}

	// set DA_Enable based on if 32-bit frame buffer or not
	if ( Frame [ Ctx ].PSM < 2 )
	{
		// 32-bit frame buffer //
		
		// only perform destination alpha test for 32-bit pixels
		DA_Enable &= 0x80000000;
	}
	else
	{
		// 16-bit frame buffer //
		
		// only perform destination alpha test for 16-bit pixels
		DA_Enable &= 0x8000;
	}
	
	
	// go ahead and get pointers into graphics buffer and z-buffer //
	
	// get frame buffer pointer
	buf32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	//buf32 = buf;
	//buf16 = (u16*) buf;
	
	// get z-buffer pointer
	zbuf32 = & ( RAM32 [ ZBufferStartOffset32 ] );
	//zbuf16 = (u16*) zbuf32;


#ifdef ALLOW_OPENCL_PS2_SETDRAWVARS


// order of values
// 0: XYOFFSET_X
// 1: SCISSOR_X
// 2: FRAME_X
// 3: ZBUF_X
// 4: ALPHA_X
// 5: TEST_X
// 6: TEXA
// 7: TEX0_X
// 8: TEX1_X
// 9: DIMX
// 10: 
// 11: 
// 12: MIPTBP1_X
// 13: MIPTBP2_X
// 14: CLAMP_X
// 15: PRIM/command/FOGCOL
// 16: PABE/FBA_X
// 17: COLCLAMP/DTHE
// 18: 
// 19: SCANMSK
// 20: XYZ
// 21: RGBAQ
// 22: UV/ST
// 23: FOG
// 24: XYZ
// 25: RGBAQ
// 26: UV/ST
// 27: FOG
// 28: XYZ
// 29: RGBAQ
// 30: UV/ST
// 31: FOG

	// put in rectangle data
	inputdata_ptr [ 0 ] = Offset [ Ctx ].Value;
	inputdata_ptr [ 1 ] = Scissor [ Ctx ].Value;
	inputdata_ptr [ 2 ] = Frame [ Ctx ].Value;
	inputdata_ptr [ 3 ] = ZBuf [ Ctx ].Value;
	inputdata_ptr [ 4 ] = ALPHA_X.Value;
	inputdata_ptr [ 5 ] = TEST_X.Value;
	inputdata_ptr [ 6 ] = GPURegsGp.TEXA.Value;
	inputdata_ptr [ 7 ] = pTEX0 [ Ctx ].Value;
	inputdata_ptr [ 8 ] = pTEX1 [ Ctx ].Value;
	inputdata_ptr [ 9 ] = GPURegsGp.DIMX;
	//inputdata_ptr [ 12 ] = GPURegsGp.MIPTBP1_X;
	//inputdata_ptr [ 13 ] = GPURegsGp.MIPTBP2_X;
	inputdata_ptr [ 14 ] = pClamp->Value;
	inputdata_ptr [ 15 ] = ullPRIM & 0x7ff;
	
	((u32*)inputdata_ptr) [ ( 15 << 1 ) + 1 ] = GPURegsGp.FOGCOL;
	((u32*)inputdata_ptr) [ ( 16 << 1 ) + 0 ] = GPURegsGp.PABE;
	((u32*)inputdata_ptr) [ ( 16 << 1 ) + 1 ] = FBA_X;
	((u32*)inputdata_ptr) [ ( 17 << 1 ) + 0 ] = GPURegsGp.COLCLAMP;
	((u32*)inputdata_ptr) [ ( 17 << 1 ) + 1 ] = GPURegsGp.DTHE;

	inputdata_ptr [ 19 ] = GPURegsGp.SCANMSK;

	inputdata_ptr [ 20 ] = xyz [ Coord0 ].Value;
//cout << "\nxyz0.Value=" << hex << xyz [ Coord0 ].Value << dec << " c0=" << Coord0;
	inputdata_ptr [ 21 ] = rgbaq [ Coord0 ].Value;
	inputdata_ptr [ 24 ] = xyz [ Coord1 ].Value;
//cout << "\nxyz1.Value=" << hex << xyz [ Coord1 ].Value << dec << " c1=" << Coord1;
	inputdata_ptr [ 25 ] = rgbaq [ Coord1 ].Value;
	inputdata_ptr [ 28 ] = xyz [ Coord2 ].Value;
	inputdata_ptr [ 29 ] = rgbaq [ Coord2 ].Value;

	// check tme
	if ( ullPRIM & 0x10 )
	{
		
		// check fst
		if ( ullPRIM & 0x100 )
		{
			// uv coords (FST=1) //
			inputdata_ptr [ 22 ] = uv [ Coord0 ].Value;
			inputdata_ptr [ 26 ] = uv [ Coord1 ].Value;
			inputdata_ptr [ 30 ] = uv [ Coord2 ].Value;
		}
		else
		{
			// st coords (FST=0) //
			inputdata_ptr [ 22 ] = st [ Coord0 ].Value;
			inputdata_ptr [ 26 ] = st [ Coord1 ].Value;
			inputdata_ptr [ 30 ] = st [ Coord2 ].Value;
		}	// end if ( inputdata_ptr [ 15 ] & 0x100 )
		
		// check fog
		if ( ullPRIM & 0x20 )
		{
			inputdata_ptr [ 23 ] = f [ Coord0 ].Value;
			inputdata_ptr [ 27 ] = f [ Coord1 ].Value;
			inputdata_ptr [ 31 ] = f [ Coord2 ].Value;
		}	// end if ( inputdata_ptr [ 15 ] & 0x20 )
	
	}	// end if ( inputdata_ptr [ 15 ] & 0x10 )

#endif

#ifdef _ENABLE_SSE41

	vFrameBuffer_WriteMask32 = _mm_set1_epi32 ( FrameBuffer_WriteMask32 );

	//vCOLCLAMP = _mm_set1_epi32( 0x000000ff | -( GPURegsGp.COLCLAMP & 1 ) );
	vCOLCLAMP = _mm_set1_epi32( 0x00ff00ff | -( GPURegsGp.COLCLAMP & 1 ) );

	if ( !Ctx )
	{
		vDATE = _mm_set1_epi32( ( GPURegsGp.TEST_1.Value & 0x4000 ) << 17 );
		vDATM = _mm_set1_epi32( ( GPURegsGp.TEST_1.Value & 0x8000 ) << 16 );
	}
	else
	{
		vDATE = _mm_set1_epi32( ( GPURegsGp.TEST_2.Value & 0x4000 ) << 17 );
		vDATM = _mm_set1_epi32( ( GPURegsGp.TEST_2.Value & 0x8000 ) << 16 );
	}

	vDestAlpha24 = _mm_set1_epi32 ( DestAlpha24 );
	vDestMask24 = _mm_set1_epi32 ( DestMask24 );
	vDA_Enable = _mm_set1_epi32 ( DA_Enable );
	vDA_Test = _mm_set1_epi32 ( DA_Test );
	vPixelOr32 = _mm_set1_epi32 ( PixelOr32 );
	vFrameBufferStartOffset32 = _mm_set1_epi32 ( FrameBufferStartOffset32 );
	vFrameBufferWidth_Pixels = _mm_set1_epi32 ( FrameBufferWidth_Pixels );
	vARef_SrcAlpha = _mm_set1_epi32 ( SrcAlpha_ARef );
	vAlphaXor32 = _mm_set1_epi32 ( AlphaXor32 );

	//fogr = ( GPURegsGp.FOGCOL >> 0 ) & 0xff;
	//fogg = ( GPURegsGp.FOGCOL >> 8 ) & 0xff;
	//fogb = ( GPURegsGp.FOGCOL >> 16 ) & 0xff;
	vFOGCOLR = _mm_set1_epi32 ( ( GPURegsGp.FOGCOL >> 0 ) & 0xff );
	vFOGCOLG = _mm_set1_epi32 ( ( GPURegsGp.FOGCOL >> 8 ) & 0xff );
	vFOGCOLB = _mm_set1_epi32 ( ( GPURegsGp.FOGCOL >> 16 ) & 0xff );

	vTA0 = _mm_set1_epi32 ( GPURegsGp.TEXA.TA0 << 24 );
	vTA1 = _mm_set1_epi32 ( GPURegsGp.TEXA.TA1 << 24 );
	
#endif


#ifdef _ENABLE_AVX2

	vFrameBuffer_WriteMask32 = _mm256_set1_epi32 ( FrameBuffer_WriteMask32 );

	vDestAlpha24 = _mm256_set1_epi32 ( DestAlpha24 );
	vDestMask24 = _mm256_set1_epi32 ( DestMask24 );
	vDA_Enable = _mm256_set1_epi32 ( DA_Enable );
	vDA_Test = _mm256_set1_epi32 ( DA_Test );
	vPixelOr32 = _mm256_set1_epi32 ( PixelOr32 );
	vFrameBufferStartOffset32 = _mm256_set1_epi32 ( FrameBufferStartOffset32 );
	vFrameBufferWidth_Pixels = _mm256_set1_epi32 ( FrameBufferWidth_Pixels );
	vARef_SrcAlpha = _mm256_set1_epi32 ( SrcAlpha_ARef );
	vAlphaXor32 = _mm256_set1_epi32 ( AlphaXor32 );

	//fogr = ( GPURegsGp.FOGCOL >> 0 ) & 0xff;
	//fogg = ( GPURegsGp.FOGCOL >> 8 ) & 0xff;
	//fogb = ( GPURegsGp.FOGCOL >> 16 ) & 0xff;
	vFOGCOLR = _mm256_set1_epi32 ( ( GPURegsGp.FOGCOL >> 0 ) & 0xff );
	vFOGCOLG = _mm256_set1_epi32 ( ( GPURegsGp.FOGCOL >> 8 ) & 0xff );
	vFOGCOLB = _mm256_set1_epi32 ( ( GPURegsGp.FOGCOL >> 16 ) & 0xff );

	vTA0 = _mm256_set1_epi32 ( GPURegsGp.TEXA.TA0 << 24 );
	vTA1 = _mm256_set1_epi32 ( GPURegsGp.TEXA.TA1 << 24 );
	
#endif
	

#if defined INLINE_DEBUG_SETDRAWVARS || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << " FrameBufferStartOffset32=" << FrameBufferStartOffset32 << " FrameBufferWidth_Pixels=" << FrameBufferWidth_Pixels;
#endif

}



void GPU::SetDrawVars_Line ( u64 *inputdata_ptr, u32 Coord0, u32 Coord1, u32 Coord2 )
{
#if defined INLINE_DEBUG_SETDRAWVARS || defined INLINE_DEBUG_PRIMITIVE
	debug << "; SetDrawVars_Line";
#endif

	u32 lContext;

	// 0: SCISSOR
	// 1: XYOFFSET
	// 2: FRAME
	// 3: ZBUF
	// 4: FBA
	// 5: TEST
	// 6: COLCLAMP
	// 7: ALPHA
	// 8: PABE
	// 9: DTHE
	// 10: DIMX
	// 11: FOGCOL
	// 12: CLAMP
	// 13: TEX0
	// 14: TEXCLUT
	// -------------
	// 15: PRIM (COMMAND)
	// 16: RGBAQ
	// 17: XYZ
	// 18: UV
	// 19: FOG
	// 20: RGBAQ
	// 21: XYZ
	// 22: UV
	// 23: FOG
	// 24: RGBAQ
	// 25: XYZ
	// 26: UV
	// 27: FOG




	if ( GPURegsGp.PRMODECONT & 1 )
	{
		// set PRIM
		inputdata_ptr [ 15 ] = GPURegsGp.PRIM.Value;
	}
	else
	{
		// set PRMODE
		inputdata_ptr [ 15 ] = ( GPURegsGp.PRIM.Value & 0x7 ) | ( GPURegsGp.PRMODE.Value & ~0x7 );
	}
	
	lContext = ( inputdata_ptr [ 15 ] >> 9 ) & 1;
	
	if ( !lContext )
	{
		// set SCISSOR
		inputdata_ptr [ 0 ] = GPURegsGp.SCISSOR_1.Value;
		
		// set XYOFFSET
		inputdata_ptr [ 1 ] = GPURegsGp.XYOFFSET_1.Value;
		
		// set FRAME
		//inputdata_ptr [ 2 ] = GPURegsGp.FRAME_1.Value;
		
		// set ZBUF
		//inputdata_ptr [ 3 ] = GPURegsGp.ZBUF_1.Value;
		
		// set FBA
		//inputdata_ptr [ 4 ] = GPURegsGp.FBA_1;
		
		// set TEST
		//inputdata_ptr [ 5 ] = GPURegsGp.TEST_1.Value;
		
		// set ALPHA
		//inputdata_ptr [ 7 ] = GPURegsGp.ALPHA_1.Value;

		
		// texture mapping info
		
		// set CLAMP
		//inputdata_ptr [ 12 ] = GPURegsGp.CLAMP_1.Value;
		
		// set TEX0
		//inputdata_ptr [ 13 ] = TEXX [ 0 ].Value;
		
	}
	else
	{
		// set SCISSOR
		inputdata_ptr [ 0 ] = GPURegsGp.SCISSOR_2.Value;
		
		// set XYOFFSET
		inputdata_ptr [ 1 ] = GPURegsGp.XYOFFSET_2.Value;
		
		// set FRAME
		//inputdata_ptr [ 2 ] = GPURegsGp.FRAME_2.Value;
		
		// set ZBUF
		//inputdata_ptr [ 3 ] = GPURegsGp.ZBUF_2.Value;
		
		// set FBA
		//inputdata_ptr [ 4 ] = GPURegsGp.FBA_2;
		
		// set TEST
		//inputdata_ptr [ 5 ] = GPURegsGp.TEST_2.Value;
		
		// set ALPHA
		//inputdata_ptr [ 7 ] = GPURegsGp.ALPHA_2.Value;
		
		
		// texture mapping info
		
		// set CLAMP
		//inputdata_ptr [ 12 ] = GPURegsGp.CLAMP_2.Value;
		
		// set TEX0
		//inputdata_ptr [ 13 ] = TEXX [ 1 ].Value;
	}

	// set COLCLAMP
	//inputdata_ptr [ 6 ] = GPURegsGp.COLCLAMP;
	
	// set PABE
	//inputdata_ptr [ 8 ] = GPURegsGp.PABE;
	
	// set DTHE
	//inputdata_ptr [ 9 ] = GPURegsGp.DTHE;
	
	// set DIMX
	//inputdata_ptr [ 10 ] = GPURegsGp.DIMX;
	
	
	inputdata_ptr [ 16 - 14 ] = rgbaq [ Coord0 ].Value;
	inputdata_ptr [ 17 - 14 ] = xyz [ Coord0 ].Value;
	inputdata_ptr [ 20 - 14 ] = rgbaq [ Coord1 ].Value;
	inputdata_ptr [ 21 - 14 ] = xyz [ Coord1 ].Value;
	inputdata_ptr [ 24 - 14 ] = rgbaq [ Coord2 ].Value;
	inputdata_ptr [ 25 - 14 ] = xyz [ Coord2 ].Value;
	

	// texture mapping info

	
	// set FOGCOL
	//inputdata_ptr [ 11 ] = GPURegsGp.FOGCOL;

	// set TEXCLUT
	//inputdata_ptr [ 14 ] = GPURegsGp.TEXCLUT.Value;
	
	// set TEXA
	//inputdata_ptr [ 31 ] = GPURegsGp.TEXA.Value;

	// check tme
	if ( inputdata_ptr [ 15 ] & 0x10 )
	{
		
	// check fst
	if ( inputdata_ptr [ 15 ] & 0x100 )
	{
		// uv coords (FST=1) //
		inputdata_ptr [ 18 - 14 ] = uv [ Coord0 ].Value;
		inputdata_ptr [ 22 - 14 ] = uv [ Coord1 ].Value;
		inputdata_ptr [ 26 - 14 ] = uv [ Coord2 ].Value;
	}
	else
	{
		// st coords (FST=0) //
		inputdata_ptr [ 18 - 14 ] = st [ Coord0 ].Value;
		inputdata_ptr [ 22 - 14 ] = st [ Coord1 ].Value;
		inputdata_ptr [ 26 - 14 ] = st [ Coord2 ].Value;
	}	// end if ( inputdata_ptr [ 15 ] & 0x100 )
	
	// check fog
	if ( inputdata_ptr [ 15 ] & 0x20 )
	{
	inputdata_ptr [ 19 - 14 ] = f [ Coord0 ].Value;
	inputdata_ptr [ 23 - 14 ] = f [ Coord1 ].Value;
	inputdata_ptr [ 27 - 14 ] = f [ Coord2 ].Value;
	}	// end if ( inputdata_ptr [ 15 ] & 0x20 )
	
	}	// end if ( inputdata_ptr [ 15 ] & 0x10 )


	// it's going to draw this immediately, so set static vars here for now
#ifdef _ENABLE_SSE41

	vCOLCLAMP = _mm_set1_epi32( 0x00ff00ff | -( GPURegsGp.COLCLAMP & 1 ) );

	vAEM = _mm_set1_epi32( GPURegsGp.TEXA.AEM );

	if ( !lContext )
	{
		vDATE = _mm_set1_epi32( ( GPURegsGp.TEST_1.Value & 0x4000 ) << 17 );
		vDATM = _mm_set1_epi32( ( GPURegsGp.TEST_1.Value & 0x8000 ) << 16 );

		switch ( ( GPURegsGp.TEST_1.Value >> 17 ) & 0x3 )
		{
			// never
			case 0:
				vZTST_LESS = _mm_set1_epi32( 0 );
				vZTST_GREATER = _mm_set1_epi32( 0 );
				vZTST_EQUAL = _mm_set1_epi32( 0 );
				break;

			// always
			case 1:
				vZTST_LESS = _mm_set1_epi32( -1 );
				vZTST_GREATER = _mm_set1_epi32( -1 );
				vZTST_EQUAL = _mm_set1_epi32( -1 );
				break;

			// greater or equal
			case 2:
				vZTST_LESS = _mm_set1_epi32( 0 );
				vZTST_GREATER = _mm_set1_epi32( -1 );
				vZTST_EQUAL = _mm_set1_epi32( -1 );
				break;

			// greater
			case 3:
				vZTST_LESS = _mm_set1_epi32( 0 );
				vZTST_GREATER = _mm_set1_epi32( -1 );
				vZTST_EQUAL = _mm_set1_epi32( 0 );
				break;
		}

		// check if ztest is even enabled
		if ( ! ( ( GPURegsGp.TEST_1.Value >> 16 ) & 1 ) )
		{
			vZTST_LESS = _mm_set1_epi32( -1 );
			vZTST_GREATER = _mm_set1_epi32( -1 );
			vZTST_EQUAL = _mm_set1_epi32( -1 );
		}


		// check if source alpha is even enabled
		vATST_AREF = _mm_set1_epi32( ( GPURegsGp.TEST_1.Value >> 4 ) & 0xff );
		vATST_MASK = _mm_setzero_si128 ();
		vATST_OFFSET = _mm_set1_epi32( -0x80000000 );
		if ( ( ( GPURegsGp.TEST_1.Value >> 0 ) & 1 ) )
		{
			// source alpha test enabled //
			//cout << "\nsource alpha1 test enabled:" << ( ( GPURegsGp.TEST_1.Value >> 1 ) & 0x7 ) << " fail:" << ( ( GPURegsGp.TEST_1.Value >> 12 ) & 0x3 );

		// source alpha test
		switch ( ( GPURegsGp.TEST_1.Value >> 1 ) & 0x7 )
		{
			// never
			case 0:
				vATST_MASK = _mm_setzero_si128 ();
				vATST_OFFSET = _mm_setzero_si128 ();
				break;

			// always
			case 1:
				vATST_MASK = _mm_setzero_si128 ();
				vATST_OFFSET = _mm_set1_epi32( -0x80000000 );
				break;

			// less
			case 2:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_set1_epi32( 0x7fffffff );
				break;

			// less or equal
			case 3:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_set1_epi32( -0x80000000 );
				break;

			// equal
			case 4:
				vATST_MASK = _mm_set1_epi32( 1 );
				vATST_OFFSET = _mm_set1_epi32( -1 );
				break;

			// greater or equal
			case 5:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_set1_epi32( -1 );
				break;

			// greater
			case 6:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_setzero_si128 ();
				break;

			// NOT equal
			case 7:
				vATST_MASK = _mm_set1_epi32( 1 );
				vATST_OFFSET = _mm_set1_epi32( 0x7fffffff );
				break;
		}


		// afail
		switch ( ( GPURegsGp.TEST_1.Value >> 12 ) & 0x3 )
		{
			// NEVER
			case 0:
				vAFAIL_PIXEL =  _mm_setzero_si128 ();
				vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				break;

			// framebuffer ONLY
			case 1:
				vAFAIL_PIXEL =  _mm_set1_epi32 ( -1 );
				vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				break;

			// zbuffer ONLY
			case 2:
				vAFAIL_PIXEL =  _mm_setzero_si128 ();
				vAFAIL_ZPIXEL =  _mm_set1_epi32 ( -1 );
				break;

			// rgb only (only for 32-bit pixels)
			case 3:
				if ( ! ( ( GPURegsGp.FRAME_1.Value >> 24 ) & 0x3f ) )
				{
					vAFAIL_PIXEL =  _mm_set1_epi32 ( 0x00ffffff );
					vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				}
				else
				{
					// otherwise leave pixels alone
					//vAFAIL_PIXEL =  _mm_set1_epi32 ( -1 );
					//vAFAIL_ZPIXEL =  _mm_set1_epi32 ( -1 );
					vAFAIL_PIXEL =  _mm_set1_epi32 ( -1 );
					vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				}

				break;

		}	// end switch ( ( GPURegsGp.TEST_1.Value >> 12 ) & 0x3 )

		}	// end if ( ( ( GPURegsGp.TEST_1.Value >> 0 ) & 1 ) )



	}
	else
	{
		vDATE = _mm_set1_epi32( ( GPURegsGp.TEST_2.Value & 0x4000 ) << 17 );
		vDATM = _mm_set1_epi32( ( GPURegsGp.TEST_2.Value & 0x8000 ) << 16 );

		switch ( ( GPURegsGp.TEST_2.Value >> 17 ) & 0x3 )
		{
			// never
			case 0:
				vZTST_LESS = _mm_set1_epi32( 0 );
				vZTST_GREATER = _mm_set1_epi32( 0 );
				vZTST_EQUAL = _mm_set1_epi32( 0 );
				break;

			// always
			case 1:
				vZTST_LESS = _mm_set1_epi32( -1 );
				vZTST_GREATER = _mm_set1_epi32( -1 );
				vZTST_EQUAL = _mm_set1_epi32( -1 );
				break;

			// greater or equal
			case 2:
				vZTST_LESS = _mm_set1_epi32( 0 );
				vZTST_GREATER = _mm_set1_epi32( -1 );
				vZTST_EQUAL = _mm_set1_epi32( -1 );
				break;

			// greater
			case 3:
				vZTST_LESS = _mm_set1_epi32( 0 );
				vZTST_GREATER = _mm_set1_epi32( -1 );
				vZTST_EQUAL = _mm_set1_epi32( 0 );
				break;
		}

		// check if ztest is even enabled
		if ( ! ( ( GPURegsGp.TEST_2.Value >> 16 ) & 1 ) )
		{
			vZTST_LESS = _mm_set1_epi32( -1 );
			vZTST_GREATER = _mm_set1_epi32( -1 );
			vZTST_EQUAL = _mm_set1_epi32( -1 );
		}

		// source alpha test
		// check if source alpha is even enabled
		vATST_AREF = _mm_set1_epi32( ( GPURegsGp.TEST_2.Value >> 4 ) & 0xff );
		vATST_MASK = _mm_setzero_si128 ();
		vATST_OFFSET = _mm_set1_epi32( -0x80000000 );
		if ( ( ( GPURegsGp.TEST_2.Value >> 0 ) & 1 ) )
		{
			// source alpha test enabled //
			//cout << "\nsource alpha2 test enabled:" << ( ( GPURegsGp.TEST_2.Value >> 1 ) & 0x7 ) << " fail:" << ( ( GPURegsGp.TEST_2.Value >> 12 ) & 0x3 );

		switch ( ( GPURegsGp.TEST_2.Value >> 1 ) & 0x7 )
		{
			// never
			case 0:
				vATST_MASK = _mm_setzero_si128 ();
				vATST_OFFSET = _mm_setzero_si128 ();
				break;

			// always
			case 1:
				vATST_MASK = _mm_setzero_si128 ();
				vATST_OFFSET = _mm_set1_epi32( -0x80000000 );
				break;

			// less
			case 2:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_set1_epi32( 0x7fffffff );
				break;

			// less or equal
			case 3:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_set1_epi32( -0x80000000 );
				break;

			// equal
			case 4:
				vATST_MASK = _mm_set1_epi32( 1 );
				vATST_OFFSET = _mm_set1_epi32( -1 );
				break;

			// greater or equal
			case 5:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_set1_epi32( -1 );
				break;

			// greater
			case 6:
				vATST_MASK = _mm_set1_epi32( -1 );
				vATST_OFFSET = _mm_setzero_si128 ();
				break;

			// NOT equal
			case 7:
				vATST_MASK = _mm_set1_epi32( 1 );
				vATST_OFFSET = _mm_set1_epi32( 0x7fffffff );
				break;

		}	// end switch ( ( GPURegsGp.TEST_2.Value >> 1 ) & 0x7 )


		// afail
		switch ( ( GPURegsGp.TEST_2.Value >> 12 ) & 0x3 )
		{
			// NEVER
			case 0:
				vAFAIL_PIXEL =  _mm_setzero_si128 ();
				vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				break;

			// framebuffer ONLY
			case 1:
				vAFAIL_PIXEL =  _mm_set1_epi32 ( -1 );
				vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				break;

			// zbuffer ONLY
			case 2:
				vAFAIL_PIXEL =  _mm_setzero_si128 ();
				vAFAIL_ZPIXEL =  _mm_set1_epi32 ( -1 );
				break;

			// rgb only (only for 32-bit pixels)
			case 3:
				// even though this is checked when drawing, still need to do this here
				if ( ! ( ( GPURegsGp.FRAME_2.Value >> 24 ) & 0x3f ) )
				{
					vAFAIL_PIXEL =  _mm_set1_epi32 ( 0x00ffffff );
					vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				}
				else
				{
					// otherwise leave pixels alone
					//vAFAIL_PIXEL =  _mm_set1_epi32 ( -1 );
					//vAFAIL_ZPIXEL =  _mm_set1_epi32 ( -1 );
					vAFAIL_PIXEL =  _mm_set1_epi32 ( -1 );
					vAFAIL_ZPIXEL =  _mm_setzero_si128 ();
				}

				break;

		}	// end switch ( ( GPURegsGp.TEST_2.Value >> 12 ) & 0x3 )

		}	// end if ( ( ( GPURegsGp.TEST_2.Value >> 0 ) & 1 ) )

	}

#endif


#if defined INLINE_DEBUG_SETDRAWVARS_LINE || defined INLINE_DEBUG_PRIMITIVE
	//debug << dec << " FrameBufferStartOffset32=" << FrameBufferStartOffset32 << " FrameBufferWidth_Pixels=" << FrameBufferWidth_Pixels;
#endif

}



void GPU::DrawPoint ( u32 Coord0 )
{
#if defined INLINE_DEBUG_POINT || defined INLINE_DEBUG_PRIMITIVE
	debug << "; DrawPoint";
#endif

	// get common variables needed to draw object
	//SetDrawVars ();
	SetDrawVars ( & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ], Coord0, 1, 2 );

	// check for some important conditions
	if ( Window_XRight < Window_XLeft )
	{
#ifdef VERBOSE_POINT
		cout << "\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
#if defined INLINE_DEBUG_POINT || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
		return;
	}
	
	if ( Window_YBottom < Window_YTop )
	{
#ifdef VERBOSE_POINT
		cout << "\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
#if defined INLINE_DEBUG_POINT || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
		return;
	}


#ifdef USE_TEMPLATES_PS2_POINT

	//u64 *inputdata_ptr;
	u64 drawvars_ptr [ 16 ];
	u64 NumPixels;

	//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );
	
	
	//SetDrawVars_Line ( inputdata_ptr, Coord0, 0, 0 );
	SetDrawVars_Line ( drawvars_ptr, Coord0, 0, 0 );
	
	//Select_RenderPoint_t ( inputdata_ptr, 0 );
	Select_RenderPoint_t ( drawvars_ptr, GPU::ulThreadedGPU );
	

	//if ( BusyUntil_Cycle < *_DebugCycleCount )
	//{
	//	BusyUntil_Cycle = *_DebugCycleCount + ( NumPixels >> 4 );
	//}

#else

	
	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	/*
	if ( ulNumberOfThreads )
	{
		// for now, wait to finish
		//while ( ulInputBuffer_Count );
		Finish ();
	}
	*/
	
#ifdef ENABLE_BUFFER_CACHE_POINT
	RenderPoint_DS_Cache ( Coord0 );
#else
	// draw single-color point //
	RenderPoint_DS ( Coord0 );
#endif

#endif
}



void GPU::DrawLine ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; DrawLine";
#endif

	

#ifdef USE_TEMPLATES_PS2_LINE
	//u64 *inputdata_ptr;
	u64 drawvars_ptr [ 16 ];
	u64 NumPixels;

	//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );

	
	//SetDrawVars_Line ( inputdata_ptr, Coord0, Coord1, 0 );
	SetDrawVars_Line ( drawvars_ptr, Coord0, Coord1, 0 );

	/*
	if ( _GPU->ulNumberOfThreads )
	{

		// send
		ulInputBuffer_WriteIndex++;
		//x64ThreadSafe::Utilities::Lock_ExchangeAdd64 ( (long long&) ulInputBuffer_WriteIndex, 1 );
	}
	else
	{
	*/

#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << " START-DRAW-LINE2";
#endif

		//NumPixels = Select_RenderLine_t ( inputdata_ptr, 0 );
		NumPixels = Select_RenderLine_t ( drawvars_ptr, GPU::ulThreadedGPU );


#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << " END-DRAW-LINE2";
#endif

		
	/*
		if ( BusyUntil_Cycle < *_DebugCycleCount )
		{
			BusyUntil_Cycle = *_DebugCycleCount + ( NumPixels >> 4 );
		}
	}
	*/
	
#else

	
	// get common variables needed to draw object
	//SetDrawVars ();
	SetDrawVars ( & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ], Coord0, Coord1, 2 );

	// check for some important conditions
	if ( Window_XRight < Window_XLeft )
	{
#ifdef VERBOSE_LINE
		cout << "\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
		return;
	}
	
	if ( Window_YBottom < Window_YTop )
	{
#ifdef VERBOSE_LINE
		cout << "\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
		return;
	}


	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	//Finish ();

	
#ifdef ENABLE_BUFFER_CACHE_LINE
	switch ( Gradient )
	{
		case 0:
			// draw single-color triangle //
			RenderLine_Mono_DS_Cache ( Coord0, Coord1 );
			break;
			
		case 1:
			// draw gradient triangle //
			RenderLine_Gradient_DS_Cache ( Coord0, Coord1 );
			break;
	}
#else
	// check if object is shaded
	switch ( Gradient )
	{
		case 0:
			// draw single-color triangle //
			RenderLine_Mono_DS ( Coord0, Coord1 );
			break;
			
		case 1:
			// draw gradient triangle //
			RenderLine_Gradient_DS ( Coord0, Coord1 );
			break;
	}
#endif

#endif

}






// need to specify what coords to use to draw object
void GPU::DrawTriangle ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; DrawTriangle";
#endif
	
	//u64 *inputdata_ptr;
	u64 drawvars_ptr [ 16 ];
	u64 NumPixels;

	
	

#ifdef USE_TEMPLATES_PS2_TRIANGLE

	
	//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );

	
	//SetDrawVars_Line ( inputdata_ptr, Coord0, Coord1, Coord2 );
	SetDrawVars_Line ( drawvars_ptr, Coord0, Coord1, Coord2 );

	/*
	if ( _GPU->ulNumberOfThreads )
	{

		// send
		ulInputBuffer_WriteIndex++;
		//x64ThreadSafe::Utilities::Lock_ExchangeAdd64 ( (long long&) ulInputBuffer_WriteIndex, 1 );
	}
	else
	{
	*/

#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "->RenderTriangle";
#endif

		//NumPixels = Select_RenderTriangle_t ( inputdata_ptr, 0 );
		NumPixels = Select_RenderTriangle_t ( drawvars_ptr, GPU::ulThreadedGPU );
		
		
	/*
		if ( BusyUntil_Cycle < *_DebugCycleCount )
		{
			BusyUntil_Cycle = *_DebugCycleCount + ( NumPixels >> 4 );
		}
	}
	*/

#else

	
	// get common variables needed to draw object
	//SetDrawVars ();
	SetDrawVars ( & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ], Coord0, Coord1, Coord2 );

	// check for some important conditions
	if ( Window_XRight < Window_XLeft )
	{
#ifdef VERBOSE_TRIANGLE
		cout << "\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
		return;
	}
	
	if ( Window_YBottom < Window_YTop )
	{
#ifdef VERBOSE_TRIANGLE
		cout << "\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
		return;
	}


	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	//Finish ();

	
	// check if object is texture mapped
	switch ( TextureMapped )
	{
		case 0:
		
#ifdef ALLOW_OPENCL_PS2_TRIANGLE_COLOR
			//cout << "\nDRAWING TEXTURED SPRITE" << " bEnable_OpenCL=" << bEnable_OpenCL;
			//bEnable_OpenCL = true;
			if ( bEnable_OpenCL )
			{
				//cout << "\nsprite0:" << " idx=" << dec << ulHwInputBuffer_WriteIndex << " xy0=" << hex << pHwInputBuffer64[24] << " xy1=" << pHwInputBuffer64[20] << " offsetxy=" << pHwInputBuffer64[0] << dec << " c0=" << Coord0 << " c1=" << Coord1;
				//Copy_VRAM_toGPU ();


				// dispatch our test command
				//FlushToHardware( 0, 1 );

				// get result
				//Copy_VRAM_toCPU ();

				// submit command
				ulHwInputBuffer_WriteIndex++;


#ifdef ALLOW_OPENCL_PS2_TRIANGLE_COLOR_TEST

#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nDrawTriangleHW";	// << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		debug << dec << " x0=" << RAM16[0] << " y0=" << RAM16[1] << " x1=" << RAM16[2] << " y1=" << RAM16[3] << " x2=" << RAM16[4] << " y2=" << RAM16[5];
		debug << hex << " rgba0=" << RAM16[6] << " rgba0=" << RAM16[7] << " rgba1=" << RAM16[8] << " rgba1=" << RAM16[9] << " rgba2=" << RAM16[10] << " rgba2=" << RAM16[11];
		/*
		debug << " StartX=" << RAM16 [ 0 ] << " StartY=" << RAM16 [ 1 ];
		debug << dec << " u0=" << RAM16 [ 2 ] << " v0=" << RAM16 [ 3 ] << " u1=" << RAM16 [ 18 ] << " v1=" << RAM16 [ 19 ];
		//debug << hex << " z0=" << z0 << " z1=" << z1;
		debug << hex << " TexBufPtr32/64=" << RAM16 [ 12 ];
		debug << dec << " TexWidth=" << RAM16 [ 15 ] << " TexHeight=" << RAM16 [ 16 ];
		debug << dec << " TexBufWidth=" << RAM16 [ 17 ];
		debug << " TexPixFmt=" << PixelFormat_Names [ RAM16 [ 5 ] ];
		debug << " Clamp_ModeX=" << RAM16 [ 13 ] << " Clamp_ModeY=" << RAM16 [ 14 ];
		//debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << " CLUTPixFmt=" << PixelFormat_Names [ RAM16 [ 6 ] ];
		//debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << " CSM=" << RAM16 [ 7 ];
		//debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << " FST=" << RAM16 [ 9 ];
		debug << " dudx=" << RAM16 [ 10 ] << " dvdy=" << RAM16 [ 11 ];
		debug << " iAnd1=" << hex << RAM16 [ 20 ] << " iShift1=" << RAM16 [ 21 ] << " iShift2=" << RAM16 [ 22 ] << " iAnd3=" << RAM16 [ 23 ] << " iShift3=" << RAM16 [ 24 ];
		debug << " TexCoordY=" << dec << RAM16 [ 25 ] << " iTYOffset=" << RAM16 [ 26 ];
		debug << " TexCoordX0=" << RAM16 [ 20 ] << " TexCoordX1=" << RAM16 [ 21 ] << " TexCoordX2=" << RAM16 [ 22 ] << " TexCoordX3=" << RAM16 [ 23 ];
		debug << " vTOffset0=" << RAM16 [ 32 ] << " vTOffset1=" << RAM16 [ 33 ] << " vTOffset2=" << RAM16 [ 34 ] << " vTOffset3=" << RAM16 [ 35 ];
		debug << " vPixelShift0=" << hex << RAM16 [ 36 ] << " vPixelShift1=" << RAM16 [ 37 ] << " vPixelShift2=" << RAM16 [ 38 ] << " vPixelShift3=" << RAM16 [ 39 ];
		debug << " vIdx0=" << RAM16 [ 40 ] << " vIdx1=" << RAM16 [ 41 ] << " vIdx2=" << RAM16 [ 42 ] << " vIdx3=" << RAM16 [ 43 ];
		//debug << dec << "\r\nDrawSpriteHW";	// << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		//debug << hex << " ABE=" << RAM16 [ 0 ] << " PABE=" << RAM16 [ 1 ] << " ALPHA=" << RAM16 [ 2 ];
		//debug << " A=" << RAM16 [ 3 ] << " B=" << RAM16 [ 4 ] << " C=" << RAM16 [ 5 ] << " D=" << RAM16 [ 6 ] << " FIX=" << RAM16[ 7 ];
		//debug << " TA0=" << RAM16 [ 8 ] << " TA1=" << RAM16 [ 9 ] << " TFX=" << RAM16[10] << " TCC=" << RAM16[11] << " bgr=" << RAM16[12];
		//debug << " SrcPixel16=" << RAM16[13] << " SrcPixel0=" << RAM16 [ 14 ] << " SrcPixel1=" << RAM16 [ 15 ] << " DstPixel0=" << RAM16 [ 16 ] << " DstPixel1=" << RAM16 [ 17 ] << " OutPixel0=" << RAM16 [ 18 ] << " OutPixel1=" << RAM16 [ 19 ];
		//debug << " vaf[3]=" << RAM16[20] << " vav[3]=" << RAM16[21];
		*/
#endif

				//cout << "\nsprite1:" << " x0=" << hex << RAM16[0] << " y0=" << RAM16[1] << " x1=" << hex << RAM16[2] << " y1=" << RAM16[3] << dec << " sx=" << RAM16[5] << " sy=" << RAM16[6] << " fpsm=" << hex << RAM16[4];
				//cout << " wx0=" << dec << RAM16[7] << " wy0=" << RAM16[8] << " wx1=" << RAM16[9] << " wy1=" << RAM16[10] << " w=" << RAM16[11];
				//cout << " ex=" << RAM16[12] << " ey=" << RAM16[13];
			//DrawTriangle_Gradient32_DS ( Coord0, Coord1, Coord2 );

#endif
			}
#else
			
			// check if this is single-color or gradient
			if ( Gradient )
			{
				DrawTriangle_Gradient32_DS ( Coord0, Coord1, Coord2 );

			}
			else
			{
			

				// draw single-color triangle //
				DrawTriangle_Mono32_DS ( Coord0, Coord1, Coord2 );

			}
#endif
			
			break;
			
		case 1:

#ifdef ALLOW_OPENCL_PS2_TRIANGLE_TEXTURE
			//cout << "\nDRAWING TEXTURED SPRITE" << " bEnable_OpenCL=" << bEnable_OpenCL;
			//bEnable_OpenCL = true;
			if ( bEnable_OpenCL )
			{

				// submit command
				ulHwInputBuffer_WriteIndex++;


#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
				//cout << "\nsprite0:" << " idx=" << dec << ulHwInputBuffer_WriteIndex << " xy0=" << hex << pHwInputBuffer64[24] << " xy1=" << pHwInputBuffer64[20] << " offsetxy=" << pHwInputBuffer64[0] << dec << " c0=" << Coord0 << " c1=" << Coord1;
				//Copy_VRAM_toGPU ();


				// dispatch our test command
				FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

				ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
				ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

				ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
				ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;

				// get result
				Copy_VRAM_toCPU ();

		debug << dec << "\r\nDrawTriangleHW";	// << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		debug << dec << " StartY0=" << RAM16[1000000+0] << " EndY0=" << RAM16[1000000+1];	// << " StartY1=" << RAM16[2] << " EndY1=" << RAM16[3];
		//debug << dec << " Line1=" << RAM16[8] << " StartX0=" << RAM16[9] << " EndX0=" << RAM16[10] << " WinX0=" << RAM16[11] << " WinX1=" << RAM16[12];
		//debug << dec << " x0=" << RAM16[12] << " y0=" << RAM16[13] << " x1=" << RAM16[14] << " y1=" << RAM16[15] << " x2=" << RAM16[16] << " y2=" << RAM16[17];
		debug << dec << " ZTE=" << RAM16[1000000+2] << " ZTST=" << RAM16[1000000+3] << " ZTST_GREATER=" << RAM16[1000000+4] << " ZTST_LESS=" << RAM16[1000000+5] << " ZTST_EQUAL=" << RAM16[1000000+6] << " v2=" << RAM16[1000000+7];
		debug << dec << " z0=" << RAM16[1000000+7] << " z1=" << RAM16[1000000+8] << " z2=" << RAM16[1000000+9];
		debug << dec << " vdz0=" << RAM16[1000000+10] << " vdz1=" << RAM16[1000000+11];
		debug << dec << " vdzdy0=" << RAM16[1000000+12] << " vdzdy1=" << RAM16[1000000+13];
		debug << dec << " vdz0=" << RAM16[1000000+14] << " vdz1=" << RAM16[1000000+15];
		debug << dec << " vdzdy0=" << RAM16[1000000+16] << " vdzdy1=" << RAM16[1000000+17];
		//debug << dec << " u0=" << RAM16[1000000+2] << " v0=" << RAM16[1000000+3] << " u1=" << RAM16[1000000+4] << " v1=" << RAM16[1000000+5] << " u2=" << RAM16[1000000+6] << " v2=" << RAM16[1000000+7];
		//debug << dec << " ds0=" << RAM16[1000000+8] << " ds1=" << RAM16[1000000+9] << " dt0=" << RAM16[1000000+10] << " dt1=" << RAM16[1000000+11];
		//debug << dec << " dsdx=" << RAM16[1000000+12] << " dtdx=" << RAM16[1000000+13];
		//debug << dec << " vds0=" << RAM16[1000000+14] << " vdt0=" << RAM16[1000000+15];
		//debug << dec << " FST=" << RAM16[1000000+16];
		debug << dec << " s0=" << RAM16[1000000+21] << " t0=" << RAM16[1000000+22] << " s1=" << RAM16[1000000+23] << " t1=" << RAM16[1000000+24] << " s2=" << RAM16[1000000+25] << " t2=" << RAM16[1000000+26];
		//debug << dec << " tx0=" << RAM16[1000000+24] << " tx1=" << RAM16[1000000+25] << " tx2=" << RAM16[1000000+26] << " tx3=" << RAM16[1000000+27];
		//debug << dec << " ty0=" << RAM16[1000000+28] << " ty1=" << RAM16[1000000+29] << " ty2=" << RAM16[1000000+30] << " ty3=" << RAM16[1000000+31];
		//debug << dec << " tx0=" << RAM16[1000000+32] << " tx1=" << RAM16[1000000+33] << " tx2=" << RAM16[1000000+34] << " tx3=" << RAM16[1000000+35];
		//debug << dec << " ty0=" << RAM16[1000000+36] << " ty1=" << RAM16[1000000+37] << " ty2=" << RAM16[1000000+38] << " ty3=" << RAM16[1000000+39];
		//debug << dec << " xa0=" << RAM16[1000000+64] << " xa1=" << RAM16[1000000+65] << " xa2=" << RAM16[1000000+66] << " xa3=" << RAM16[1000000+67];
		debug << hex << " p0=" << RAM16[1000000+68] << " p0=" << RAM16[1000000+69] << " p1=" << RAM16[1000000+70] << " p1=" << RAM16[1000000+71];
		debug << hex << " p2=" << RAM16[1000000+72] << " p2=" << RAM16[1000000+73] << " p3=" << RAM16[1000000+74] << " p3=" << RAM16[1000000+75];
		//" iV1=" << RAM16[19] << " iV0=" << RAM16[20] << " Temp=" << RAM16[21];
		//debug << dec << " denominator=" << RAM16[22] << " t0=" << RAM16[20] << " t1=" << RAM16[21];
		//debug << dec << " denominator=" << RAM16[14] << " X0Index=" << RAM16[15] << " X1Index=" << RAM16[16];
		//debug << hex << " rgba0=" << RAM16[6] << " rgba0=" << RAM16[7] << " rgba1=" << RAM16[8] << " rgba1=" << RAM16[9] << " rgba2=" << RAM16[10] << " rgba2=" << RAM16[11];
		/*
		debug << " StartX=" << RAM16 [ 0 ] << " StartY=" << RAM16 [ 1 ];
		debug << dec << " u0=" << RAM16 [ 2 ] << " v0=" << RAM16 [ 3 ] << " u1=" << RAM16 [ 18 ] << " v1=" << RAM16 [ 19 ];
		//debug << hex << " z0=" << z0 << " z1=" << z1;
		debug << hex << " TexBufPtr32/64=" << RAM16 [ 12 ];
		debug << dec << " TexWidth=" << RAM16 [ 15 ] << " TexHeight=" << RAM16 [ 16 ];
		debug << dec << " TexBufWidth=" << RAM16 [ 17 ];
		debug << " TexPixFmt=" << PixelFormat_Names [ RAM16 [ 5 ] ];
		debug << " Clamp_ModeX=" << RAM16 [ 13 ] << " Clamp_ModeY=" << RAM16 [ 14 ];
		//debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << " CLUTPixFmt=" << PixelFormat_Names [ RAM16 [ 6 ] ];
		//debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << " CSM=" << RAM16 [ 7 ];
		//debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << " FST=" << RAM16 [ 9 ];
		debug << " dudx=" << RAM16 [ 10 ] << " dvdy=" << RAM16 [ 11 ];
		debug << " iAnd1=" << hex << RAM16 [ 20 ] << " iShift1=" << RAM16 [ 21 ] << " iShift2=" << RAM16 [ 22 ] << " iAnd3=" << RAM16 [ 23 ] << " iShift3=" << RAM16 [ 24 ];
		debug << " TexCoordY=" << dec << RAM16 [ 25 ] << " iTYOffset=" << RAM16 [ 26 ];
		debug << " TexCoordX0=" << RAM16 [ 20 ] << " TexCoordX1=" << RAM16 [ 21 ] << " TexCoordX2=" << RAM16 [ 22 ] << " TexCoordX3=" << RAM16 [ 23 ];
		debug << " vTOffset0=" << RAM16 [ 32 ] << " vTOffset1=" << RAM16 [ 33 ] << " vTOffset2=" << RAM16 [ 34 ] << " vTOffset3=" << RAM16 [ 35 ];
		debug << " vPixelShift0=" << hex << RAM16 [ 36 ] << " vPixelShift1=" << RAM16 [ 37 ] << " vPixelShift2=" << RAM16 [ 38 ] << " vPixelShift3=" << RAM16 [ 39 ];
		debug << " vIdx0=" << RAM16 [ 40 ] << " vIdx1=" << RAM16 [ 41 ] << " vIdx2=" << RAM16 [ 42 ] << " vIdx3=" << RAM16 [ 43 ];
		//debug << dec << "\r\nDrawSpriteHW";	// << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		//debug << hex << " ABE=" << RAM16 [ 0 ] << " PABE=" << RAM16 [ 1 ] << " ALPHA=" << RAM16 [ 2 ];
		//debug << " A=" << RAM16 [ 3 ] << " B=" << RAM16 [ 4 ] << " C=" << RAM16 [ 5 ] << " D=" << RAM16 [ 6 ] << " FIX=" << RAM16[ 7 ];
		//debug << " TA0=" << RAM16 [ 8 ] << " TA1=" << RAM16 [ 9 ] << " TFX=" << RAM16[10] << " TCC=" << RAM16[11] << " bgr=" << RAM16[12];
		//debug << " SrcPixel16=" << RAM16[13] << " SrcPixel0=" << RAM16 [ 14 ] << " SrcPixel1=" << RAM16 [ 15 ] << " DstPixel0=" << RAM16 [ 16 ] << " DstPixel1=" << RAM16 [ 17 ] << " OutPixel0=" << RAM16 [ 18 ] << " OutPixel1=" << RAM16 [ 19 ];
		//debug << " vaf[3]=" << RAM16[20] << " vav[3]=" << RAM16[21];
		*/
#endif

				//cout << "\nsprite1:" << " x0=" << hex << RAM16[0] << " y0=" << RAM16[1] << " x1=" << hex << RAM16[2] << " y1=" << RAM16[3] << dec << " sx=" << RAM16[5] << " sy=" << RAM16[6] << " fpsm=" << hex << RAM16[4];
				//cout << " wx0=" << dec << RAM16[7] << " wy0=" << RAM16[8] << " wx1=" << RAM16[9] << " wy1=" << RAM16[10] << " w=" << RAM16[11];
				//cout << " ex=" << RAM16[12] << " ey=" << RAM16[13];
			//DrawTriangle_GradientTexture32_DS ( Coord0, Coord1, Coord2 );
			}
#else

			if ( Gradient )
			{
				DrawTriangle_GradientTexture32_DS ( Coord0, Coord1, Coord2 );
			}
			else
			{
				// draw texture-mapped triangle //
				DrawTriangle_Texture32_DS ( Coord0, Coord1, Coord2 );
			}

#endif

			break;
	}

#endif
}




void GPU::DrawSprite ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; DrawSprite";
	debug << "; (before)BusyUntil_Cycle=" << dec << BusyUntil_Cycle;
#endif

	// sprite is being drawn, so will start a new primitive after this
	StartPrimitive ();


	
	//u64 *inputdata_ptr;
	u64 drawvars_ptr [ 16 ];
	u64 NumPixels;

	
	
	
#ifdef USE_TEMPLATES_PS2_RECTANGLE

	//inputdata_ptr = & ( inputdata [ ( ulInputBuffer_WriteIndex & c_ulInputBuffer_Mask ) << c_ulInputBuffer_Shift ] );

	SetDrawVars_Line ( drawvars_ptr, Coord0, Coord1, 0 );

	/*
	if ( _GPU->ulNumberOfThreads )
	{

		// send
		ulInputBuffer_WriteIndex++;
		//x64ThreadSafe::Utilities::Lock_ExchangeAdd64 ( (long long&) ulInputBuffer_WriteIndex, 1 );
	}
	else
	{
	*/
			//NumPixels = Select_RenderSprite_t ( inputdata_ptr, 0 );
			NumPixels = Select_RenderSprite_t ( drawvars_ptr, GPU::ulThreadedGPU );
			
			
	/*
			if ( BusyUntil_Cycle < *_DebugCycleCount )
			{
				BusyUntil_Cycle = *_DebugCycleCount + ( NumPixels >> 4 );
			}
	}
	*/

#else

	
	// get common variables needed to draw object
	//SetDrawVars ();
	SetDrawVars ( & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ], Coord0, Coord1, 2 );

	// check for some important conditions
	if ( Window_XRight < Window_XLeft )
	{
#ifdef VERBOSE_SPRITE
		cout << "\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_XRight < Window_XLeft.\n";
#endif
		return;
	}
	
	if ( Window_YBottom < Window_YTop )
	{
#ifdef VERBOSE_SPRITE
		cout << "\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
		debug << "\r\nhps2x64 ALERT: GPU: Window_YBottom < Window_YTop.\n";
#endif
		return;
	}


	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	//Finish ();

	
	// check if object is texture mapped
	switch ( TextureMapped )
	{
		case 0:

			// draw single-color rectangle //


			// if testing opengl/compute, then test that code here
#ifdef ALLOW_OPENCL_PS2_RECTANGLE
			//cout << "\nDRAWING RECTANGLE\n";
			//bEnable_OpenCL = true;
			if ( bEnable_OpenCL )
			{
				//Copy_VRAM_toGPU ();


				// dispatch our test command
				//FlushToHardware( 0, 1 );

				// get result
				//Copy_VRAM_toCPU ();

				// submit command
				ulHwInputBuffer_WriteIndex++;

#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST

				/*
				// dispatch our test command
				FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

				ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
				ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

				ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
				ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;
				*/

				// get result
				//Copy_VRAM_toCPU ();

				//RenderRectangle_DS ( Coord0, Coord1 );

				//Copy_VRAM_toGPU ();
#endif
			}
			//else
#else
			{
			RenderRectangle_DS ( Coord0, Coord1 );
			}
#endif

			break;
			
		case 1:

			// draw texture-mapped sprite //
			

#ifdef ALLOW_OPENCL_PS2_SPRITE
			//cout << "\nDRAWING TEXTURED SPRITE" << " bEnable_OpenCL=" << bEnable_OpenCL;
			//bEnable_OpenCL = true;
			if ( bEnable_OpenCL )
			{

				// submit command
				ulHwInputBuffer_WriteIndex++;

#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
				// dispatch our test command
				/*
				FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

				ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
				ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

				ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
				ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;


				// get result
				Copy_VRAM_toCPU ();
				*/

		/*
		debug << dec << "\r\nDrawSpriteHW";	// << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		debug << hex << " p0=" << RAM16[1000000+0] << " p1=" << RAM16[1000000+1] << " p2=" << RAM16[1000000+2] << " p3=" << RAM16[1000000+3];
		debug << " StartX=" << RAM16 [ 0 ] << " StartY=" << RAM16 [ 1 ];
		debug << dec << " u0=" << RAM16 [ 2 ] << " v0=" << RAM16 [ 3 ] << " u1=" << RAM16 [ 18 ] << " v1=" << RAM16 [ 19 ];
		//debug << hex << " z0=" << z0 << " z1=" << z1;
		debug << hex << " TexBufPtr32/64=" << RAM16 [ 12 ];
		debug << dec << " TexWidth=" << RAM16 [ 15 ] << " TexHeight=" << RAM16 [ 16 ];
		debug << dec << " TexBufWidth=" << RAM16 [ 17 ];
		debug << " TexPixFmt=" << PixelFormat_Names [ RAM16 [ 5 ] ];
		debug << " Clamp_ModeX=" << RAM16 [ 13 ] << " Clamp_ModeY=" << RAM16 [ 14 ];
		//debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << " CLUTPixFmt=" << PixelFormat_Names [ RAM16 [ 6 ] ];
		//debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << " CSM=" << RAM16 [ 7 ];
		//debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << " FST=" << RAM16 [ 9 ];
		debug << " dudx=" << RAM16 [ 10 ] << " dvdy=" << RAM16 [ 11 ];
		debug << " iAnd1=" << hex << RAM16 [ 20 ] << " iShift1=" << RAM16 [ 21 ] << " iShift2=" << RAM16 [ 22 ] << " iAnd3=" << RAM16 [ 23 ] << " iShift3=" << RAM16 [ 24 ];
		debug << " TexCoordY=" << dec << RAM16 [ 25 ] << " iTYOffset=" << RAM16 [ 26 ];
		debug << " TexCoordX0=" << RAM16 [ 20 ] << " TexCoordX1=" << RAM16 [ 21 ] << " TexCoordX2=" << RAM16 [ 22 ] << " TexCoordX3=" << RAM16 [ 23 ];
		debug << " vTOffset0=" << RAM16 [ 32 ] << " vTOffset1=" << RAM16 [ 33 ] << " vTOffset2=" << RAM16 [ 34 ] << " vTOffset3=" << RAM16 [ 35 ];
		debug << " vPixelShift0=" << hex << RAM16 [ 36 ] << " vPixelShift1=" << RAM16 [ 37 ] << " vPixelShift2=" << RAM16 [ 38 ] << " vPixelShift3=" << RAM16 [ 39 ];
		debug << " vIdx0=" << RAM16 [ 40 ] << " vIdx1=" << RAM16 [ 41 ] << " vIdx2=" << RAM16 [ 42 ] << " vIdx3=" << RAM16 [ 43 ];
		*/

		/*
		debug << dec << "\r\nDrawSpriteHW";	// << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		//debug << hex << " ABE=" << RAM16 [ 0 ] << " PABE=" << RAM16 [ 1 ] << " ALPHA=" << RAM16 [ 2 ];
		//debug << " A=" << RAM16 [ 3 ] << " B=" << RAM16 [ 4 ] << " C=" << RAM16 [ 5 ] << " D=" << RAM16 [ 6 ] << " FIX=" << RAM16[ 7 ];
		//debug << " TA0=" << RAM16 [ 8 ] << " TA1=" << RAM16 [ 9 ] << " TFX=" << RAM16[10] << " TCC=" << RAM16[11] << " bgr=" << RAM16[12];
		debug << hex << " SrcPixel0=" << RAM16 [1000000+0] << " SrcPixel0=" << RAM16 [1000000+1] << " SrcPixel1=" << RAM16 [1000000+2] << " SrcPixel1=" << RAM16 [1000000+3];
		debug << hex << " SrcPixel2=" << RAM16 [1000000+4] << " SrcPixel2=" << RAM16 [1000000+5] << " SrcPixel3=" << RAM16 [1000000+6] << " SrcPixel3=" << RAM16 [1000000+7];
		debug << " tx0=" << RAM16[1000000+8]  << " tx1=" << RAM16[1000000+9]  << " tx2=" << RAM16[1000000+10]  << " tx3=" << RAM16[1000000+11];
		debug << " ty=" << RAM16[1000000+12] << " vu0=" << RAM16[1000000+13] << " vv0=" << RAM16[1000000+14] << " vu1=" << RAM16[1000000+15];
		debug << " u0=" << RAM16[1000000+16]  << " u1=" << RAM16[1000000+17]  << " u2=" << RAM16[1000000+18]  << " u3=" << RAM16[1000000+19];
		debug << " x0=" << RAM16[1000000+20]  << " x1=" << RAM16[1000000+21]  << " x2=" << RAM16[1000000+22]  << " x3=" << RAM16[1000000+23];

				RenderSprite_DS ( Coord0, Coord1 );

				Copy_VRAM_toGPU ();
				*/

#endif

				//cout << "\nsprite1:" << " x0=" << hex << RAM16[0] << " y0=" << RAM16[1] << " x1=" << hex << RAM16[2] << " y1=" << RAM16[3] << dec << " sx=" << RAM16[5] << " sy=" << RAM16[6] << " fpsm=" << hex << RAM16[4];
				//cout << " wx0=" << dec << RAM16[7] << " wy0=" << RAM16[8] << " wx1=" << RAM16[9] << " wy1=" << RAM16[10] << " w=" << RAM16[11];
				//cout << " ex=" << RAM16[12] << " ey=" << RAM16[13];
			//RenderSprite_DS ( Coord0, Coord1 );
			}
			//else
#else
			{
			RenderSprite_DS ( Coord0, Coord1 );
			}
#endif

			break;
	}
#endif


	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; (after)BusyUntil_Cycle=" << dec << BusyUntil_Cycle;
#endif
}




u32 GPU::Get_PageNumber( u32 PageOffset, s32 x, s32 y, u32 BufferWidthInBlocks, u32 PixelWidth )
{
	s32 xBlock, yBlock;
	
	u32 yShift;
	
	u32 PageIndex;
	
	yShift = 5 + ( ( PixelWidth >> 5 ) ^ 1 );
	
	xBlock = x >> 6;
	yBlock = y >> yShift;
	
	PageIndex = PageOffset + xBlock + ( yBlock * BufferWidthInBlocks );
	
	return PageIndex;
}






void GPU::Cache_Page ( u32 StartPageIndex, u32 PageRow, u32 PageFormat, u32 BufferWidthInBlocks )
{
	static const int c_iCacheWidth = 64;
	
	static const int c_iCacheHeight32 = 32;
	static const int c_iCacheHeight16 = 64;
	
	static const int c_iNumPages = 512;
	static const int c_iNumPages_Mask = c_iNumPages - 1;
	
	int iCacheHeight, iCacheWidth;
	
	int ix, iy;
	u32 Pixel;
	
	u32 BufferStartOffset32;
	u32 RowStartPageIndex;
	
	u32 ulFormatSig;
	
	union
	{
		u32* cache_buf32;
		u16* cache_buf16;
	};

	union
	{
		u32* real_buf32;
		u16* real_buf16;
	};

	
	// get start index for that row of pages
	RowStartPageIndex = StartPageIndex + ( PageRow * BufferWidthInBlocks );


#ifdef DEBUG_PAGE_CACHE
	debug << "\r\nCache_Page";
	debug << " StartPageIndex=" << dec << StartPageIndex;
	debug << " PageRow=" << dec << PageRow;
	debug << " PageFormat=" << dec << PageFormat;
	debug << " BufferWidthInBlocks=" << dec << BufferWidthInBlocks;
	debug << " RowStartPageIndex=" << dec << RowStartPageIndex;
#endif
	
	
	ulFormatSig = ( ( PageFormat & 0x3f ) | ( RowStartPageIndex << 6 ) | ( BufferWidthInBlocks << ( 6 + 9 ) ) );
	
	// if page is already cached, then if the page format is different need to uncache first
	if ( ulCachedBufFormat [ ( RowStartPageIndex ) & c_iNumPages_Mask ] != ulFormatSig )
	{
		// check if page is cached at all
		if ( ( ulCachedBufFormat [ ( RowStartPageIndex ) & c_iNumPages_Mask ] & 0x3f ) != 0x3f )
		{
#ifdef DEBUG_PAGE_CACHE
	debug << " PAGE-PREVIOUSLY-CACHED";
#endif

			// un-cache the buffer format that was previously cached for page index
			Uncache_Page ( RowStartPageIndex );
		}

#ifdef DEBUG_PAGE_CACHE
	debug << " CACHING-ROW";
#endif
		
		// set the buffer format being cached for page (for entire row)
		for ( int i = 0; i < BufferWidthInBlocks; i++ )
		{
			ulCachedBufFormat [ ( RowStartPageIndex + i ) & c_iNumPages_Mask ] = ulFormatSig;
		}
	}
	else
	{
#ifdef DEBUG_PAGE_CACHE
	debug << " ROW-ALREADY-CACHED";
#endif

		return;
	}
	
	
	BufferStartOffset32 = ( RowStartPageIndex & 0x1ff ) << 11;
	
	real_buf32 = & ( _GPU->RAM32 [ BufferStartOffset32 ] );
	cache_buf32 = & ( _GPU->BUF32 [ BufferStartOffset32 ] );
	
	
	switch ( PageFormat & 2 )
	{
		case 0:
			iCacheHeight = c_iCacheHeight32;
			break;
			
		case 2:
			iCacheHeight = c_iCacheHeight16;
			break;
	}
	
	// get the width of buffer in pixels
	iCacheWidth = BufferWidthInBlocks << 6;
	
	for ( iy = 0; iy < iCacheHeight; iy++ )
	{
		for ( ix = 0; ix < iCacheWidth; ix++ )
		{
			if ( cache_buf32 >= PtrEndC )
			{
				return;
			}
			
			switch ( PageFormat )
			{
				case 0:
				case 1:
					*cache_buf32++ = real_buf32 [ CvtAddrPix32 ( ix, iy, 0 ) ];
					break;
					
				case 2:
					*cache_buf16++ = real_buf16 [ CvtAddrPix16 ( ix, iy, 0 ) ];
					break;
					
				case 0xa:
					*cache_buf16++ = real_buf16 [ CvtAddrPix16S ( ix, iy, 0 ) ];
					break;

				// PSMZ32
				case 0x30:
				
				// PSMZ24
				case 0x31:
					*cache_buf32++ = real_buf32 [ CvtAddrZBuf32 ( ix, iy, 0 ) ];
					break;

				// PSMZ16
				case 0x32:
					*cache_buf16++ = real_buf16 [ CvtAddrZBuf16 ( ix, iy, 0 ) ];
					break;
					
				// PSMZ16S
				case 0x3a:
					*cache_buf16++ = real_buf16 [ CvtAddrZBuf16S ( ix, iy, 0 ) ];
					break;
					
				default:
					cout << "\nhps2x64: GPU: CACHE: ALERT: invalid frame-buffer format! FBBUF PSM=" << hex << PageFormat << "\n";
					break;
					
			}
		}
	}
	
}

void GPU::Uncache_Page ( u32 PageIndex )
{
	static const int c_iCacheWidth = 64;
	
	static const int c_iCacheHeight32 = 32;
	static const int c_iCacheHeight16 = 64;

	static const int c_iNumPages = 512;
	static const int c_iNumPages_Mask = c_iNumPages - 1;
	
	int iCacheHeight, iCacheWidth;
	
	int ix, iy;
	u32 Pixel;
	
	u32 BufferStartOffset32;
	
	s32 PageFormat;
	u32 RowStartPageIndex;
	
	u32 BufferWidthInBlocks;
	
	
	union
	{
		u32* cache_buf32;
		u16* cache_buf16;
	};

	union
	{
		u32* real_buf32;
		u16* real_buf16;
	};

#ifdef DEBUG_PAGE_CACHE
	debug << "\r\nUncache_Page";
	debug << " PageIndex=" << dec << PageIndex;
#endif

	PageIndex &= c_iNumPages_Mask;

	PageFormat = ulCachedBufFormat [ PageIndex ] & 0x3f;
	
	
	// if page is not cached then return (already un-cached)
	if ( PageFormat == 0x3f )
	{
#ifdef DEBUG_PAGE_CACHE
	debug << " PAGE-NOT-CACHED";
#endif

		return;
	}
	

	RowStartPageIndex = ( ulCachedBufFormat [ PageIndex ] >> 6 ) & 0x1ff;

	BufferWidthInBlocks = ( ulCachedBufFormat [ PageIndex ] >> ( 6 + 9 ) );


#ifdef DEBUG_PAGE_CACHE
	debug << " PAGE-CACHED";
	debug << " RowStartPageIndex=" << dec << RowStartPageIndex;
	debug << " BufferWidthInBlocks=" << dec << BufferWidthInBlocks;
#endif


#ifdef DEBUG_PAGE_CACHE
	debug << " UNCACHING-ROW";
#endif
	
	// set buffer as uncached (for entire row)
	for ( int i = 0; i < BufferWidthInBlocks; i++ )
	{
		ulCachedBufFormat [ ( RowStartPageIndex + i ) & c_iNumPages_Mask ] = -1;
	}
	
	
	
	BufferStartOffset32 = RowStartPageIndex << 11;
	
	real_buf32 = & ( _GPU->RAM32 [ BufferStartOffset32 ] );
	cache_buf32 = & ( _GPU->BUF32 [ BufferStartOffset32 ] );
	
	
	switch ( PageFormat & 2 )
	{
		case 0:
			iCacheHeight = c_iCacheHeight32;
			break;
			
		case 2:
			iCacheHeight = c_iCacheHeight16;
			break;
	}
	
	
	// get the width in pixels for buffer
	iCacheWidth = BufferWidthInBlocks << 6;
	
	
	for ( iy = 0; iy < iCacheHeight; iy++ )
	{
		for ( ix = 0; ix < iCacheWidth; ix++ )
		{
			if ( cache_buf32 >= PtrEndC )
			{
				return;
			}
			
			switch ( PageFormat )
			{
				case 0:
				case 1:
					real_buf32 [ CvtAddrPix32 ( ix, iy, 0 ) ] = *cache_buf32++;
					break;
					
				case 2:
					real_buf16 [ CvtAddrPix16 ( ix, iy, 0 ) ] = *cache_buf16++;
					break;
					
				case 0xa:
					real_buf16 [ CvtAddrPix16S ( ix, iy, 0 ) ] = *cache_buf16++;
					break;

				// PSMZ32
				case 0x30:
				
				// PSMZ24
				case 0x31:
					real_buf32 [ CvtAddrZBuf32 ( ix, iy, 0 ) ] = *cache_buf32++;
					break;

				// PSMZ16
				case 0x32:
					real_buf16 [ CvtAddrZBuf16 ( ix, iy, 0 ) ] = *cache_buf16++;
					break;
					
				// PSMZ16S
				case 0x3a:
					real_buf16 [ CvtAddrZBuf16S ( ix, iy, 0 ) ] = *cache_buf16++;
					break;
					
				default:
					cout << "\nhps2x64: GPU: UNCACHE: ALERT: invalid frame-buffer format! FBBUF PSM=" << hex << PageFormat << "\n";
					break;
					
			}
		}
	}

}



// rendering functions //



// 3-d drawing //



// transfer functions //




// does ( A - B ) * C + D
u32 GPU::AlphaABCD_32 ( u32 A, u32 B, u32 C, u32 D )
{
	u32 uResult;
	s32 cA, cB, cC, cD, sC1, sC2, sC3;
	
	// get alpha value
	cC = ( C >> 24 ) & 0xff;
	
	// get component 1
	cA = ( A >> 0 ) & 0xff;
	cB = ( B >> 0 ) & 0xff;
	cD = ( D >> 0 ) & 0xff;
	
	// the blending does not clamp anything actually, and the result here actually does not get clamped until after dithering
	//sC1 = SignedClamp<s32,8>( ( ( cA - cB ) * cC ) >> 7 );
	sC1 = ( ( ( cA - cB ) * cC ) >> 7 );
	
	// add with D color
	sC1 += cD;
	
	// if COLCLAMP then clamp, otherwise just mask to 8-bits
	if ( GPURegsGp.COLCLAMP & 1 )
	{
		sC1 = SignedClamp<s32,8> ( sC1 );
	}
	else
	{
		sC1 &= 0xff;
	}
	
	
	// get component 2
	cA = ( A >> 8 ) & 0xff;
	cB = ( B >> 8 ) & 0xff;
	cD = ( D >> 8 ) & 0xff;
	
	//sC2 = SignedClamp<s32,8>( ( ( cA - cB ) * cC ) >> 7 );
	sC2 = ( ( ( cA - cB ) * cC ) >> 7 );
	
	// add with D color
	sC2 += cD;
	
	// if COLCLAMP then clamp, otherwise just mask to 8-bits
	if ( GPURegsGp.COLCLAMP & 1 )
	{
		sC2 = SignedClamp<s32,8> ( sC2 );
	}
	else
	{
		sC2 &= 0xff;
	}
	
	
	// get component 3
	cA = ( A >> 16 ) & 0xff;
	cB = ( B >> 16 ) & 0xff;
	cD = ( D >> 16 ) & 0xff;
	
	//sC3 = SignedClamp<s32,8>( ( ( cA - cB ) * cC ) >> 7 );
	sC3 = ( ( ( cA - cB ) * cC ) >> 7 );

	// add with D color
	sC3 += cD;
	
	// if COLCLAMP then clamp, otherwise just mask to 8-bits
	if ( GPURegsGp.COLCLAMP & 1 )
	{
		sC3 = SignedClamp<s32,8> ( sC3 );
	}
	else
	{
		sC3 &= 0xff;
	}
	
	
	return sC1 | ( sC2 << 8 ) | ( sC3 << 16 );
}



u32 GPU::AlphaABCD_16 ( u32 A, u32 B, u32 C, u32 D )
{
	u32 uResult;
	s32 cA, cB, cC, cD, sC1, sC2, sC3;
	
	// get alpha
	cC = ( C >> 24 ) & 0xff;
	
	// get component 1
	cA = ( A >> 0 ) & 0x1f;
	cB = ( B >> 0 ) & 0x1f;
	cD = ( D >> 0 ) & 0x1f;
	
	// the blending does not clamp anything actually, and the result here actually does not get clamped until after dithering
	//sC1 = SignedClamp<s32,5>( ( ( cA - cB ) * cC ) >> 7 );
	sC1 = ( ( ( cA - cB ) * cC ) >> 7 );
	
	// add with D color
	sC1 += cD;
	
	// if COLCLAMP then clamp, otherwise just mask to 8-bits
	if ( GPURegsGp.COLCLAMP & 1 )
	{
		sC1 = SignedClamp<s32,5> ( sC1 );
	}
	else
	{
		sC1 &= 0x1f;
	}
	
	// get component 2
	cA = ( A >> 5 ) & 0x1f;
	cB = ( B >> 5 ) & 0x1f;
	cD = ( D >> 5 ) & 0x1f;
	
	//sC2 = SignedClamp<s32,5>( ( ( cA - cB ) * cC ) >> 7 );
	sC2 = ( ( ( cA - cB ) * cC ) >> 7 );
	
	// add with D color
	sC2 += cD;
	
	// if COLCLAMP then clamp, otherwise just mask to 8-bits
	if ( GPURegsGp.COLCLAMP & 1 )
	{
		sC2 = SignedClamp<s32,5> ( sC2 );
	}
	else
	{
		sC2 &= 0x1f;
	}
	
	
	// get component 3
	cA = ( A >> 10 ) & 0x1f;
	cB = ( B >> 10 ) & 0x1f;
	cD = ( D >> 10 ) & 0x1f;
	
	//sC3 = SignedClamp<s32,5>( ( ( cA - cB ) * cC ) >> 7 );
	sC3 = ( ( ( cA - cB ) * cC ) >> 7 );
	
	// add with D color
	sC3 += cD;
	
	// if COLCLAMP then clamp, otherwise just mask to 8-bits
	if ( GPURegsGp.COLCLAMP & 1 )
	{
		sC3 = SignedClamp<s32,5> ( sC3 );
	}
	else
	{
		sC3 &= 0x1f;
	}
	
	
	return sC1 | ( sC2 << 5 ) | ( sC3 << 10 );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GPU::DebugWindow_Enable ()
{

#ifndef _CONSOLE_DEBUG_ONLY_

	static constexpr char* DebugWindow_Caption = "PS2 FrameBuffers Debug Window";
	static constexpr int DebugWindow_X = 10;
	static constexpr int DebugWindow_Y = 10;
	static constexpr int DebugWindow_Width = c_iFrameBuffer_DisplayWidth;
	static constexpr int DebugWindow_Height = c_iFrameBuffer_DisplayHeight;
	
	int i;
	long xsize, ysize;
	stringstream ss;
	
	cout << "\nGPU::DebugWindow_Enable";
	
	if ( !DebugWindow_Enabled )
	{
		// create the main debug window
		xsize = DebugWindow_Width;
		ysize = DebugWindow_Height;
		FrameBuffer_DebugWindow = new WindowClass::Window ();
		FrameBuffer_DebugWindow->GetRequiredWindowSize ( &xsize, &ysize, FALSE );
		FrameBuffer_DebugWindow->Create ( DebugWindow_Caption, DebugWindow_X, DebugWindow_Y, xsize /*DebugWindow_Width*/, ysize /*DebugWindow_Height + 50*/ );
		FrameBuffer_DebugWindow->DisableCloseButton ();
		
		cout << "\nFramebuffer: xsize=" << xsize << "; ysize=" << ysize;
		FrameBuffer_DebugWindow->GetWindowSize ( &xsize, &ysize );
		cout << "\nWindow Size. xsize=" << xsize << "; ysize=" << ysize;
		FrameBuffer_DebugWindow->GetViewableArea ( &xsize, &ysize );
		cout << "\nViewable Size. xsize=" << xsize << "; ysize=" << ysize;
		
		cout << "\nCreated main debug window";
		
		/////////////////////////////////////////////////////////
		// enable opengl for the frame buffer window
		FrameBuffer_DebugWindow->EnableOpenGL ();
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho (0, DebugWindow_Width, DebugWindow_Height, 0, 0, 1);
		glMatrixMode (GL_MODELVIEW);

		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		// this window is no longer the window we want to draw to
		FrameBuffer_DebugWindow->OpenGL_ReleaseWindow ();
		
		DebugWindow_Enabled = true;
		
		cout << "\nEnabled opengl for frame buffer window";

		// update the value lists
		DebugWindow_Update ();
	}
	
		cout << "\n->GPU::DebugWindow_Enable";

#endif

}

void GPU::DebugWindow_Disable ()
{

#ifndef _CONSOLE_DEBUG_ONLY_

	if ( DebugWindow_Enabled )
	{
		delete FrameBuffer_DebugWindow;
	
		// disable debug window
		DebugWindow_Enabled = false;
	}
	
#endif

}

void GPU::DebugWindow_Update ()
{

#ifndef _CONSOLE_DEBUG_ONLY_

	if ( DebugWindow_Enabled )
	{
		_GPU->Draw_FrameBuffers ();
	}
	
#endif

}







// ----------------------------------------------------------------------------------------------------------
// New functions for data structure in GPU

void GPU::RenderPoint_DS ( u32 Coord0 )
{
#if defined INLINE_DEBUG_POINT || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderPoint";
#endif

	// render in 8bits per pixel, then convert down when done
	
	static const int c_iVectorSize = 1;

	u32 NumberOfPixelsDrawn;
	
	// if writing 16-bit pixels, then this could change to a u16 pointer
	//u32 *ptr, *buf;
	
	u32 bgr;
	//u32 DestPixel, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0;
	
	// must be 64-bit signed
	s64 z0;
	
	//u32 *buf32;
	//u16 *buf16;
	
	//u32 *ptr32;
	//u16 *ptr16;
	
	// array for alpha selection
	//u32 AlphaSelect [ 4 ];

	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	//u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	//u32 RGB24_TAlpha;
	
	// alpha blending selection
	//u32 uA, uB, uC, uD;
	
	// pabe
	//u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	//u32 PixelOr32, PixelOr16;
	
	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	//u32 DA_Enable, DA_Test;
	
	// source alpha test
	//u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	//u32 Alpha_Fail;
	//u32 SrcAlphaTest_Pass, SrcAlpha_ARef;

	// depth test
	//s64 DepthTest_Offset;

	// in a 24-bit frame buffer, destination alpha is always 0x80
	//u32 DestAlpha24, DestMask24;

	
	// z-buffer
	//u32 *zbuf32;
	//u16 *zbuf16;
	//u32 *zptr32;
	//u16 *zptr16;
	
	//u32 ZBuffer_Shift, ZBuffer_32bit;
	//s64 ZBufValue;
	
	

	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	
	// get z
	z0 = ( (u64) xyz [ Coord0 ].Z );
	
	
	// check is 32-bit or 24-bit z value
	// this is handled earlier by making 24-bit z-values into 32-bit z-values

#ifdef ENABLE_INVERT_ZVALUE
	// ?? it must be using the reciprocal since z-buffer test passes when value is greater ??
	// do the division if greater than one, since if it is 1, you get a 33-bit value
	// but if you just inverted the bits? It's not like it's the z-value used for perspective correction or anything..
	z0 ^= 0xffffffffULL;
#endif
	
	
	// get fill color (should be the last color set)
	//bgr = rgbaq [ Coord0 ].Value & 0xffffffffL;
	bgr = rgbaq_Current.Value & 0xffffffffL;
	
#if defined INLINE_DEBUG_POINT || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	
	// coords are in 12.4 fixed point
	// must round to nearest for point
	//x0 >>= 4;
	//y0 >>= 4;
	x0 = ( x0 + 0x8 ) >> 4;
	y0 = ( y0 + 0x8 ) >> 4;
	
#if defined INLINE_DEBUG_POINT || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0;
#endif


	// check if sprite is within draw area
	if ( x0 < Window_XLeft || x0 > Window_XRight || y0 < Window_YTop || y0 > Window_YBottom ) return;
#if defined INLINE_DEBUG_POINT_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawPoint" << " FinalCoords: x0=" << x0 << " y0=" << y0;
		debug << " z0=" << hex << z0;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << hex << " FBuf=" << (FrameBufferStartOffset32 >> 11);
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif


#ifdef VERBOSE_ZTE
	// depth test
	if ( !TEST_X.ZTE )
	{
		cout << "\nhps2x64: ALERT: GPU: ZTE is zero!!!\n";
	}
#endif

	
	// for points, already did all the needed window checks at this point //
	
	NumberOfPixelsDrawn = 1;
	
	// need to convert bgr to 16-bit pixel if it is a 16-bit frame buffer
	if ( FrameBuffer_PixelFormat > 1 )
	{
		// 16-bit frame buffer //
		
		// note: this actually needs to be done ahead of time, the conversion from 32-bit pixel to 16-bit pixel
		// convert to 16-bit pixel
		// must also convert alpha
		bgr = Color24To16 ( bgr ) | ( ( bgr >> 16 ) & 0x8000 ) | ( bgr & 0xff000000 );
		
		// set the alpha
		//AlphaSelect [ 0 ] = bgr | ( AlphaSelect [ 0 ] & 0xff000000 );
		AlphaSelect [ 0 ] = bgr;
	}
	
	// PlotPixel_Mono
	// input: x, y, z, bgr
	PlotPixel_Mono ( x0, y0, z0, bgr );
	
}


void GPU::RenderLine_Mono_DS ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderLine_Mono";
#endif

	// render in 8bits per pixel, then convert down when done
	
	static const int c_iVectorSize = 1;

	s32 StartX, EndX, StartY, EndY;
	//u32 PixelsPerLine;
	u32 NumberOfPixelsDrawn;
	
	// if writing 16-bit pixels, then this could change to a u16 pointer
	//u32 *ptr, *buf32;
	//u16 *buf16;
	
	u32 bgr;
	//u32 DestPixel, bgr, bgr_temp;
	//u32 bgr2;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	
	s64 z0, z1;
	
	s32 Line, x_across;

	
	s32 distance, x_distance, y_distance;
	
	s64 dxdc;
	s64 dydc;
	s64 line_x, line_y;
	
	// and for the z
	s64 dzdc;
	s64 line_z;
	
	//u32 DestPixel, PixelMask = 0;
	s32 iX, iY;
	
	//u32 *ptr32;
	//u16 *ptr16;

	/*
	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];

	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	*/
	
	// z-buffer
	/*
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;
	*/

	/*
	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;

	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	*/
	
	/*
	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;
	*/

	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;

	
	NumberOfPixelsDrawn = 0;

	
	/*
	// get frame buffer pointer
	buf32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	buf16 = (u16*) buf32;
	
	// get z-buffer pointer
	zbuf32 = & ( RAM32 [ ZBufferStartOffset32 ] );
	zbuf16 = (u16*) zbuf32;
	*/
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// need to invert the z's for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
#endif
	
	// get fill color
	//bgr = rgbaq [ Coord0 ].Value & 0xffffffffL;
	bgr = rgbaq_Current.Value & 0xffffffffL;
	
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	// coords are in 12.4 fixed point
	x0 >>= 4;
	y0 >>= 4;
	x1 >>= 4;
	y1 >>= 4;
	
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif


	// check if line is within draw area
//	if ( x1 < Window_XLeft || x0 > Window_XRight || y1 < Window_YTop || y0 > Window_YBottom ) return;
#if defined INLINE_DEBUG_LINE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
//	else
//	{
		debug << dec << "\r\nDrawLine_Mono" << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
//	}
#endif

	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	// on PS2, coords are in 12.4 fixed point
	StartX = x0;
	EndX = x1;
	StartY = y0;
	EndY = y1;

	if ( StartY < Window_YTop )
	{
		StartY = Window_YTop;
	}
	
	if ( EndY > ((s32)Window_YBottom) )
	{
		EndY = Window_YBottom;
	}
	
	if ( StartX < ((s32)Window_XLeft) )
	{
		StartX = Window_XLeft;
	}
	
	if ( EndX > ((s32)Window_XRight) )
	{
		EndX = Window_XRight;
	}

	
	// ***todo*** draw the line //

	// get size of line
	x_distance = _Abs( x1 - x0 );
	y_distance = _Abs( y1 - y0 );
	if ( x_distance > y_distance ) distance = x_distance; else distance = y_distance;
	
	// going from y0 to y1, get the change in y for every change in count
	if ( distance )
	{
		dxdc = ( ( (s64)x1 - (s64)x0 ) << 32 ) / distance;
		dydc = ( ( (s64)y1 - (s64)y0 ) << 32 ) / distance;
		
		dzdc = ( ( (u64)z1 - (u64)z0 ) << 23 ) / distance;
	}
	
	// init line x,y
	line_x = ( ( (s64) x0 ) << 32 );
	line_y = ( ( (s64) y0 ) << 32 );
	
	// init line z
	line_z = ( ( (u64) z0 ) << 23 );
	
	// if a 16-bit framebuffer, then need to convert pixel from 32-bit to 16-bit
	if ( FrameBuffer_PixelFormat > 1 )
	{
		// 16-bit frame buffer //
		
		// note: this actually needs to be done ahead of time, the conversion from 32-bit pixel to 16-bit pixel
		// convert to 16-bit pixel
		// must also convert alpha
		bgr = Color24To16 ( bgr ) | ( ( bgr >> 16 ) & 0x8000 ) | ( bgr & 0xff000000 );
		
		// set the alpha
		//AlphaSelect [ 0 ] = bgr | ( AlphaSelect [ 0 ] & 0xff000000 );
		AlphaSelect [ 0 ] = bgr;
	}


	// draw the line
	// note: don't draw the end-point
	//for ( u32 i = 0; i <= distance; i++ )
	for ( u32 i = 0; i < distance; i++ )
	{
		// get x coord
		iX = ( _Round( line_x ) >> 32 );
		
		// get y coord
		iY = ( _Round( line_y ) >> 32 );
		
		
		if ( iX >= Window_XLeft && iY >= Window_YTop
		&& iX <= Window_XRight && iY <= Window_YBottom )
		{
			// plot the pixel
			PlotPixel_Mono ( iX, iY, line_z >> 23, bgr );
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn++;
		}
		
		line_x += dxdc;
		line_y += dydc;
		
		line_z += dzdc;
	}
	
}


void GPU::RenderLine_Gradient_DS ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderLine_Gradient";
#endif

	//static constexpr double dShadedLine_50_CyclesPerPixel = 2.0L;

	s32 StartX, EndX, StartY, EndY;
	//u32 PixelsPerLine;
	u32 NumberOfPixelsDrawn;
	
	// if writing 16-bit pixels, then this could change to a u16 pointer
	//u32 *ptr, *buf32;
	//u16 *buf16;
	
	u32 bgr;
	//u32 DestPixel, bgr, bgr_temp;
	//u32 bgr2;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	u32 bgr0, bgr1;
	
	s64 z0, z1;
	
	s32 Line, x_across;
	
	
	s32 distance, x_distance, y_distance;
	
	s32 r0, g0, b0, r1, g1, b1;
	s32 a0, a1;
	
	s64 dxdc;
	s64 dydc;
	s64 drdc, dgdc, dbdc;
	s64 line_x, line_y;
	s64 line_r, line_g, line_b;
	
	// ***TODO*** interpolate alpha
	s64 dadc;
	s64 line_a;

	// and for the z
	s64 dzdc;
	s64 line_z;
	
	
	//u32 DestPixel, PixelMask = 0;
	
	s32 iX, iY;
	
	//u32 *ptr32;
	//u16 *ptr16;

	/*
	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];
	
	u32 PixelAlpha, PixelAlpha16;


	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	
	// z-buffer
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;

	
	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;

	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	*/


	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	union
	{
		u32 *zptr32;
		u16 *zptr16;
	};
	
	
	/*
	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;
	
	PixelAlpha = rgbaq_Current.Value & 0xff000000;
	PixelAlpha16 = ( PixelAlpha >> 16 ) & 0x8000;
	*/

	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	
	NumberOfPixelsDrawn = 0;

	
	/*
	// get frame buffer pointer
	buf32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	buf16 = (u16*) buf32;
	*/
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// invert z for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
#endif
	
	// get fill color
	//bgr = rgbaq_Current.Value & 0xffffffffL;
	bgr0 = rgbaq [ Coord0 ].Value & 0xffffffffL;
	bgr1 = rgbaq [ Coord1 ].Value & 0xffffffffL;


	// get color components
	r0 = bgr0 & 0xff;
	r1 = bgr1 & 0xff;
	g0 = ( bgr0 >> 8 ) & 0xff;
	g1 = ( bgr1 >> 8 ) & 0xff;
	b0 = ( bgr0 >> 16 ) & 0xff;
	b1 = ( bgr1 >> 16 ) & 0xff;

	// get alpha
	a0 = ( bgr0 >> 24 ) & 0xff;
	a1 = ( bgr1 >> 24 ) & 0xff;
	
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	// coords are in 12.4 fixed point
	x0 >>= 4;
	y0 >>= 4;
	x1 >>= 4;
	y1 >>= 4;
	
#if defined INLINE_DEBUG_LINE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif




	// check if line is within draw area
//	if ( x1 < Window_XLeft || x0 > Window_XRight || y1 < Window_YTop || y0 > Window_YBottom ) return;
#if defined INLINE_DEBUG_LINE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
//	else
//	{
		debug << dec << "\r\nDrawLine_Gradient" << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
//	}
#endif

	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	// on PS2, coords are in 12.4 fixed point
	StartX = x0;
	EndX = x1;
	StartY = y0;
	EndY = y1;

	if ( StartY < Window_YTop )
	{
		StartY = Window_YTop;
	}
	
	if ( EndY > ((s32)Window_YBottom) )
	{
		EndY = Window_YBottom;
	}
	
	if ( StartX < ((s32)Window_XLeft) )
	{
		StartX = Window_XLeft;
	}
	
	if ( EndX > ((s32)Window_XRight) )
	{
		EndX = Window_XRight;
	}

	
	// ***todo*** draw the line //

	// get size of line
	x_distance = _Abs( x1 - x0 );
	y_distance = _Abs( y1 - y0 );
	if ( x_distance > y_distance ) distance = x_distance; else distance = y_distance;
	
	// going from y0 to y1, get the change in y for every change in count
	if ( distance )
	{
		dxdc = ( ( (s64)x1 - (s64)x0 ) << 32 ) / distance;
		dydc = ( ( (s64)y1 - (s64)y0 ) << 32 ) / distance;
		drdc = ( ( (s64)r1 - (s64)r0 ) << 32 ) / distance;
		dgdc = ( ( (s64)g1 - (s64)g0 ) << 32 ) / distance;
		dbdc = ( ( (s64)b1 - (s64)b0 ) << 32 ) / distance;
		
		// get dzdc for z
		dzdc = ( ( (s64)z1 - (s64)z0 ) << 23 ) / distance;
		
		// get dadc for alpha
		dadc = ( ( (s64)a1 - (s64)a0 ) << 32 ) / distance;
	}
	
	// init line x,y
	line_x = ( ( (s64) x0 ) << 32 );
	line_y = ( ( (s64) y0 ) << 32 );
	line_r = ( ( (u64) r0 ) << 32 );
	line_g = ( ( (u64) g0 ) << 32 );
	line_b = ( ( (u64) b0 ) << 32 );
	
	// init line z
	line_z = ( ( (u64) z0 ) << 23 );
	
	// init alpha for line
	line_a = ( ( (u64) a0 ) << 32 );
	
	
	// draw the line
	// note: don't draw the end-point
	//for ( u32 i = 0; i <= distance; i++ )
	for ( u32 i = 0; i < distance; i++ )
	{
		// get x coord
		iX = ( _Round( line_x ) >> 32 );
		
		// get y coord
		iY = ( _Round( line_y ) >> 32 );


		
		
		if ( iX >= Window_XLeft && iY >= Window_YTop
		&& iX <= Window_XRight && iY <= Window_YBottom )
		{
			bgr = ( _Round( line_r ) >> 32 ) | ( ( _Round( line_g ) >> 32 ) << 8 ) | ( ( _Round( line_b ) >> 32 ) << 16 ) | ( ( _Round( line_a ) >> 32 ) << 24 );
			
			
			// plot the pixel
			PlotPixel_Gradient ( iX, iY, line_z >> 23, bgr );
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn++;
		}
		
		line_x += dxdc;
		line_y += dydc;
		line_r += drdc;
		line_g += dgdc;
		line_b += dbdc;
		
		line_z += dzdc;
		line_a += dadc;
	}
	
}


void GPU::RenderRectangle_DS ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderRectangle";
#endif

	// render in 8bits per pixel, then convert down when done
	
	static const int c_iVectorSize = 1;

	s32 StartX, EndX, StartY, EndY;
	u32 PixelsPerLine;
	u32 NumberOfPixelsDrawn;
	
	// if writing 16-bit pixels, then this could change to a u16 pointer
	//u32 *ptr, *buf;
	//u32 *ptr32;
	//u16 *ptr16;
	
	//u32 *buf32;
	//u16 *buf16;
	
	u32 DestPixel, bgr, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	
	// and the z coords
	s64 z0, z1;
	
	s32 Line, x_across;
	
	// interpolate the z ??
	s64 dzdx, dzdy;
	s64 iZ;
	
	s64 Temp;
	
	/*
	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];


	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	
	// z-buffer
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;

	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;
	*/

	
	/*
	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	
	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;
	*/

	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	/*
	// get frame buffer pointer
	buf = & ( RAM32 [ FrameBufferStartOffset32 ] );
	buf32 = (u32*) buf;
	buf16 = (u16*) buf;
	*/
	
	// looks like it is possible to have coord1 on the left and coord0 on the right
	//if ( ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y ) || ( xyz [ Coord1 ].X < xyz [ Coord0 ].X ) )
	//{
	//	// swap
	//	Swap ( Coord1, Coord0 );
	//}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	x1 = xyz [ Coord1 ].X;
	y0 = xyz [ Coord0 ].Y;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// invert z for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
#endif

	// z0 should be same as z1 ??
	z0 = z1;
	
	// get dzdx/dzdy for z
	// ***TODO*** this is incorrect since sometimes the x/y coords get switched. Need to fix
	//if ( x1 - x0 ) dzdx = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( x1 - x0 ) );
	//if ( y1 - y0 ) dzdy = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( y1 - y0 ) );

	
	// get fill color
	//bgr = rgbaq [ Coord0 ].Value & 0xffffffffL;
	bgr = rgbaq_Current.Value & 0xffffffffL;




	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << (x0>>4) << " y0=" << (y0>>4) << " x1=" << (x1>>4) << " y1=" << (y1>>4);
#endif

	// looks like some sprites have y1 < y0 and/or x1 < x0
	// didn't expect that so need to alert and figure out some other time
	if ( ( y1 < y0 ) || ( x1 < x0 ) )
	{
//#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
//		debug << dec << "\r\nERROR: y1 < y0 or x1 < x0 in rectangle!!!";
//		debug << dec << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
//#endif

		StartX = ( x0 <= x1 ) ? x0 : x1;
		EndX = ( x0 <= x1 ) ? x1 : x0;
		StartY = ( y0 <= y1 ) ? y0 : y1;
		EndY = ( y0 <= y1 ) ? y1 : y0;
		
		x0 = StartX;
		x1 = EndX;
		y0 = StartY;
		y1 = EndY;

//#ifdef VERBOSE_SPRITE_ERROR
//		cout << "\nhps2x64: GPU: ERROR: Rectangle has x1 < x0 or y1 < y0!!!";
//#endif
//
//		return;
	}

	
	// the coords on the left and top are kept when equal (12.4 fixed point)
	//x0 += 0xf;
	//y0 += 0xf;
	
	// the coords on the right and bottom are not kept when equal (12.4 fixed point)
	//x1 -= 1;
	//y1 -= 1;
	

	// coords are in 12.4 fixed point
	//x0 >>= 4;
	//y0 >>= 4;
	//x1 >>= 4;
	//y1 >>= 4;
	StartX = ( x0 + 0xf ) >> 4;
	EndX = ( x1 - 1 ) >> 4;
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	

	// check if sprite is within draw area
	//if ( x1 < Window_XLeft || x0 > Window_XRight || y1 < Window_YTop || y0 > Window_YBottom ) return;
	if ( EndX < Window_XLeft || StartX > Window_XRight || EndY < Window_YTop || StartY > Window_YBottom )
	{
		//debug << dec << "\r\nDrawRectangle->OUTSIDE WINDOW" << dec << " StartX=" << StartX << " EndX=" << EndX << " StartY=" << StartY << " EndY=" << EndY;
		//debug << " Window_XLeft=" << Window_XLeft << " Window_XRight=" << Window_XRight << " Window_YTop=" << Window_YTop << " Window_YBottom=" << Window_YBottom;
		return;
	}
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawRectangle" << " FinalCoords: x0=" << (x0>>4) << " y0=" << (y0>>4) << " x1=" << (x1>>4) << " y1=" << (y1>>4);
		debug << hex << " bgr=" << bgr;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " uA=" << uA;
		debug << " uB=" << uB;
		debug << " uC=" << uC;
		debug << " uD=" << uD;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif


	/*
	// *** TODO *** this needs to be removed!!!
	if ( pTest->ATE )
	{
		// alpha test is enabled
		
		if ( ( pTest->AFAIL != 0x1 ) && ( pTest->AFAIL != 0x3 ) )
		{
			// alpha test is not set to draw into frame buffer on test fail
			// not implemented yet
			return;
		}
	}
	*/
	

	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	// z is starting at z0 for now
	//z0 <<= 23;
	
	// on PS2, coords are in 12.4 fixed point
	//StartX = x0;
	//EndX = x1;
	//StartY = y0;
	//EndY = y1;

	//if ( StartY < Window_YTop )
	//{
	//	StartY = Window_YTop;
	//}
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		//v0 += ( Window_YTop - StartY ) * dvdy;
		Temp += ( Window_YTop - StartY ) << 4;
		StartY = Window_YTop;
	}
	
	// also update z
	//z0 += ( dzdy >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		EndY = Window_YBottom;
	}
	
	//if ( StartX < Window_XLeft )
	//{
	//	StartX = Window_XLeft;
	//}
	Temp = ( StartX << 4 ) - x0;
	
	if ( StartX < Window_XLeft )
	{
		//u0 += ( Window_XLeft - StartX ) * dudx;
		Temp += ( Window_XLeft - StartX ) << 4;
		StartX = Window_XLeft;
	}
	
	// also update z
	//z0 += ( dzdx >> 4 ) * Temp;
	
	
	if ( EndX > Window_XRight )
	{
		EndX = Window_XRight;
	}

	
	// check that there is a pixel to draw
	if ( ( EndX < StartX ) || ( EndY < StartY ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: EndY < StartY or EndX < StartX in rectangle!!!";
		debug << dec << " FinalCoords: StartX=" << StartX << " StartY=" << StartY << " EndX=" << EndX << " EndY=" << EndY;
#endif

		return;
	}
	
	NumberOfPixelsDrawn = ( EndX - StartX + 1 ) * ( EndY - StartY + 1 );
	
	// there are probably multiple pixel pipelines, so  might need to divide by like 8 or 16 or something
	if ( BusyUntil_Cycle < *_DebugCycleCount )
	{
		BusyUntil_Cycle = *_DebugCycleCount + ( NumberOfPixelsDrawn >> 4 );
	}
	//else
	//{
	//	BusyUntil_Cycle += NumberOfPixelsDrawn;
	//}
	
	// need to convert bgr to 16-bit pixel if it is a 16-bit frame buffer
	if ( FrameBuffer_PixelFormat > 1 )
	{
		// 16-bit frame buffer //
		
		// note: this actually needs to be done ahead of time, the conversion from 32-bit pixel to 16-bit pixel
		// convert to 16-bit pixel
		// must also convert alpha
		bgr = Color24To16 ( bgr ) | ( ( bgr >> 16 ) & 0x8000 ) | ( bgr & 0xff000000 );
		
		// set the alpha
		//AlphaSelect [ 0 ] = bgr | ( AlphaSelect [ 0 ] & 0xff000000 );
		AlphaSelect [ 0 ] = bgr;
	}
	
	
	// looks like the endy value is not included
	//for ( Line = StartY; Line <= EndY; Line++ )
	for ( Line = StartY; Line <= EndY; Line++ )
	{
#if defined INLINE_DEBUG_RECTANGLE_PIXEL
	debug << "\r\ny=" << dec << Line;
#endif

		//iZ = z0;

		for ( x_across = StartX; x_across <= EndX /* && ptr32 < PtrEnd */; x_across += c_iVectorSize )
		{
#if defined INLINE_DEBUG_RECTANGLE_PIXEL
	debug << " x=" << dec << x_across;
	debug << " VB=" << dec << lVBlank;
	debug << " OFS=" << dec << CvtAddrPix32 ( x_across, Line, FrameBufferWidth_Pixels );
#endif

			// ***TODO*** ***PROBLEM*** need to interpolate z ??
			// plot the pixel
			//PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );
			PlotPixel_Mono ( x_across, Line, z1, bgr );

			//iZ += dzdx;
		}
		
		//z0 += dzdy;
	}
	
}


#ifdef _ENABLE_SSE41

void GPU::RenderRectangle_DSx4 ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderRectangle";
#endif

	// render in 8bits per pixel, then convert down when done
	
	static const int c_iVectorSize = 4;

	s32 StartX, EndX, StartY, EndY;
	u32 NumberOfPixelsDrawn;
	
	
	u32 DestPixel, bgr, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	
	// and the z coords
	s64 z0, z1;
	
	s32 Line, x_across;
	
	// interpolate the z ??
	//s64 dzdx, dzdy;
	//s64 iZ;
	
	s64 Temp;
	
	__m128i vEnable;
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 ( 3, 2, 1, 0 );
	
	__m128i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m128i vXInc;
	__m128i vYInc;
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vYInc = _mm_set1_epi32 ( 1 );


	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	x1 = xyz [ Coord1 ].X;
	y0 = xyz [ Coord0 ].Y;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	

	// z0 should be same as z1 ??
	z0 = z1;
	
	

	
	// get fill color
	bgr = rgbaq_Current.Value & 0xffffffffL;

	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif

	// looks like some sprites have y1 < y0 and/or x1 < x0
	// didn't expect that so need to alert and figure out some other time
	if ( ( y1 < y0 ) || ( x1 < x0 ) )
	{

		StartX = ( x0 <= x1 ) ? x0 : x1;
		EndX = ( x0 <= x1 ) ? x1 : x0;
		StartY = ( y0 <= y1 ) ? y0 : y1;
		EndY = ( y0 <= y1 ) ? y1 : y0;
		
		x0 = StartX;
		x1 = EndX;
		y0 = StartY;
		y1 = EndY;

	}

	
	

	// coords are in 12.4 fixed point
	StartX = ( x0 + 0xf ) >> 4;
	EndX = ( x1 - 1 ) >> 4;
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	

	// check if sprite is within draw area
	if ( EndX < Window_XLeft || StartX > Window_XRight || EndY < Window_YTop || StartY > Window_YBottom ) return;
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawRectangle" << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
		debug << hex << " bgr=" << bgr;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		if ( Alpha )
		{
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		}
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif


	

	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		Temp += ( Window_YTop - StartY ) << 4;
		StartY = Window_YTop;
	}
	
	
	
	if ( EndY > Window_YBottom )
	{
		EndY = Window_YBottom;
	}
	
	Temp = ( StartX << 4 ) - x0;
	
	if ( StartX < Window_XLeft )
	{
		Temp += ( Window_XLeft - StartX ) << 4;
		StartX = Window_XLeft;
	}
	
	
	
	if ( EndX > Window_XRight )
	{
		EndX = Window_XRight;
	}

	
	// check that there is a pixel to draw
	if ( ( EndX < StartX ) || ( EndY < StartY ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: EndY < StartY or EndX < StartX in rectangle!!!";
		debug << dec << " FinalCoords: StartX=" << StartX << " StartY=" << StartY << " EndX=" << EndX << " EndY=" << EndY;
#endif

		return;
	}
	
	NumberOfPixelsDrawn = ( EndX - StartX + 1 ) * ( EndY - StartY + 1 );
	
	// there are probably multiple pixel pipelines, so  might need to divide by like 8 or 16 or something
	if ( BusyUntil_Cycle < *_DebugCycleCount )
	{
		BusyUntil_Cycle = *_DebugCycleCount + ( NumberOfPixelsDrawn >> 4 );
	}

	
	
	vYInc = _mm_set1_epi32 ( 1 );
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vBgr = _mm_set1_epi32 ( bgr );
	vStartX = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
	vYacross = _mm_set1_epi32 ( StartY );
	vZacross = _mm_set1_epi32 ( z1 );
	vXmax = _mm_set1_epi32 ( EndX + 1 );
	
	vAlphaSelect [ 0 ] = _mm_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm_set1_epi32 ( AlphaSelect [ 3 ] );
	
	// looks like the endy value is not included
	for ( Line = StartY; Line <= EndY; Line++ )
	{
#if defined INLINE_DEBUG_RECTANGLE_PIXEL
	debug << "\r\ny=" << dec << Line;
#endif

		vXacross = vStartX;

		for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
		{
#if defined INLINE_DEBUG_RECTANGLE_PIXEL
	debug << " x=" << dec << x_across;
	debug << " OFS=" << dec << CvtAddrPix32 ( x_across, Line, FrameBufferWidth_Pixels );
#endif

			vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );

			// ***TODO*** ***PROBLEM*** need to interpolate z ??
			// plot the pixel
			//PlotPixel_Mono ( x_across, Line, z1, bgr );
			PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

			vXacross = _mm_add_epi32 ( vXacross, vXInc );
		}
		
		vYacross = _mm_add_epi32 ( vYacross, vYInc );
	}
	
}

#endif

#ifdef _ENABLE_AVX2_RECTANGLE

void GPU::RenderRectangle_DSx8 ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderRectangle";
#endif

	// render in 8bits per pixel, then convert down when done
	
	static const int c_iVectorSize = 8;

	s32 StartX, EndX, StartY, EndY;
	u32 NumberOfPixelsDrawn;
	
	
	u32 DestPixel, bgr, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	
	// and the z coords
	s64 z0, z1;
	
	s32 Line, x_across;
	
	// interpolate the z ??
	//s64 dzdx, dzdy;
	//s64 iZ;
	
	s64 Temp;
	
	__m256i vEnable;
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 ( 7, 6, 5, 4, 3, 2, 1, 0 );
	
	__m256i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m256i vXInc;
	__m256i vYInc;
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vYInc = _mm256_set1_epi32 ( 1 );


	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	x1 = xyz [ Coord1 ].X;
	y0 = xyz [ Coord0 ].Y;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	

	// z0 should be same as z1 ??
	z0 = z1;
	
	

	
	// get fill color
	bgr = rgbaq_Current.Value & 0xffffffffL;

	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif

	// looks like some sprites have y1 < y0 and/or x1 < x0
	// didn't expect that so need to alert and figure out some other time
	if ( ( y1 < y0 ) || ( x1 < x0 ) )
	{

		StartX = ( x0 <= x1 ) ? x0 : x1;
		EndX = ( x0 <= x1 ) ? x1 : x0;
		StartY = ( y0 <= y1 ) ? y0 : y1;
		EndY = ( y0 <= y1 ) ? y1 : y0;
		
		x0 = StartX;
		x1 = EndX;
		y0 = StartY;
		y1 = EndY;

	}

	
	

	// coords are in 12.4 fixed point
	StartX = ( x0 + 0xf ) >> 4;
	EndX = ( x1 - 1 ) >> 4;
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	

	// check if sprite is within draw area
	if ( EndX < Window_XLeft || StartX > Window_XRight || EndY < Window_YTop || StartY > Window_YBottom ) return;
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawRectangle" << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
		debug << hex << " bgr=" << bgr;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		if ( Alpha )
		{
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		}
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif


	

	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		Temp += ( Window_YTop - StartY ) << 4;
		StartY = Window_YTop;
	}
	
	
	
	if ( EndY > Window_YBottom )
	{
		EndY = Window_YBottom;
	}
	
	Temp = ( StartX << 4 ) - x0;
	
	if ( StartX < Window_XLeft )
	{
		Temp += ( Window_XLeft - StartX ) << 4;
		StartX = Window_XLeft;
	}
	
	
	
	if ( EndX > Window_XRight )
	{
		EndX = Window_XRight;
	}

	
	// check that there is a pixel to draw
	if ( ( EndX < StartX ) || ( EndY < StartY ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: EndY < StartY or EndX < StartX in rectangle!!!";
		debug << dec << " FinalCoords: StartX=" << StartX << " StartY=" << StartY << " EndX=" << EndX << " EndY=" << EndY;
#endif

		return;
	}
	
	NumberOfPixelsDrawn = ( EndX - StartX + 1 ) * ( EndY - StartY + 1 );
	
	// there are probably multiple pixel pipelines, so  might need to divide by like 8 or 16 or something
	if ( BusyUntil_Cycle < *_DebugCycleCount )
	{
		BusyUntil_Cycle = *_DebugCycleCount + ( NumberOfPixelsDrawn >> 4 );
	}

	
	
	vYInc = _mm256_set1_epi32 ( 1 );
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vBgr = _mm256_set1_epi32 ( bgr );
	vStartX = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
	vYacross = _mm256_set1_epi32 ( StartY );
	vZacross = _mm256_set1_epi32 ( z1 );
	vXmax = _mm256_set1_epi32 ( EndX + 1 );
	
	vAlphaSelect [ 0 ] = _mm256_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm256_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm256_set1_epi32 ( AlphaSelect [ 3 ] );
	
	// looks like the endy value is not included
	for ( Line = StartY; Line <= EndY; Line++ )
	{
#if defined INLINE_DEBUG_RECTANGLE_PIXEL
	debug << "\r\ny=" << dec << Line;
#endif

		vXacross = vStartX;

		for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
		{
#if defined INLINE_DEBUG_RECTANGLE_PIXEL
	debug << " x=" << dec << x_across;
	debug << " OFS=" << dec << CvtAddrPix32 ( x_across, Line, FrameBufferWidth_Pixels );
#endif

			//vEnable = _mm256_cmplt_epi32 ( vXacross, vXmax );
			vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );

			// ***TODO*** ***PROBLEM*** need to interpolate z ??
			// plot the pixel
			//PlotPixel_Mono ( x_across, Line, z1, bgr );
			PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );

			vXacross = _mm256_add_epi32 ( vXacross, vXInc );
		}
		
		vYacross = _mm256_add_epi32 ( vYacross, vYInc );
	}
	
}

#endif


void GPU::RenderSprite_DS ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderSprite";
#endif

	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 1;

	// notes: looks like sprite size is same as specified by w/h

	//u32 Pixel,
	
	u16 *CLUT_LUT;
	
	u32 TexelIndex;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u8 *ptr_texture8;
	u16 *ptr_texture16;
	u32 *ptr_texture32;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;

	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	s32 StartX, EndX, StartY, EndY;
	u32 NumberOfPixelsDrawn;
	
	
	u32 *ptr32;
	u16 *ptr16;
	
	u32 *zptr32;
	u16 *zptr16;
	
	u32 DestPixel, bgr, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	s32 u0, v0, u1, v1;

	// and the z coords
	s64 z0, z1;
	
	// interpolate the z ??
	s64 dzdx, dzdy;
	s64 iZ;
	
	// interpolate f for fog
	s64 f0, f1;
	s64 dfdx, dfdy;
	s64 iF;
	
	
	s32 iU, iV, dudx, dvdy;
	
	s32 Line, x_across;
	
	u32 c1, c2, c3, ca;
	
	s64 Temp;
	
	//u32 tge;
	
	//u32 DestPixel, PixelMask = 0, SetPixelMask = 0;
	s32 TexCoordX, TexCoordY;
	
	u32 Shift1 = 0, Shift2 = 0, And1 = 0, And2 = 0;
	u32 Shift0 = 0;
	u32 TextureOffset;
	
	u32 And3, Shift3, Shift4;
	
	u32 PixelsPerLine;
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;

	

	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	

	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;

	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	
	//TexWidth_Shift = TEX0 [ Ctx ].TW;
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	//TextureOffset = ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 );
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	// this is the offset / 16
	// note: this is actually the position in the temporary buffer, not in the local memory
	CLUTOffset = TEX0 [ Ctx ].CSA;

	

	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif

	//tge = command_tge;
	//if ( ( bgr & 0x00ffffff ) == 0x00808080 ) tge = 1;
	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	//u32 ClutOffset = ( clut_x << 4 ) + ( clut_y << 10 );
	
	//clut_xoffset = clut_x << 4;
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	// *** TODO *** use a union
	ptr_texture8 = (u8*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture32 = (u32*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	// texture pixel format variables //
	
	
	
	
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			
		// PSMCT24
		case 1:
			
		// PSMCT16
		case 2:
			
		// PSMCT16S
		case 0xa:
			
		// PSMT8
		case 0x13:
			
		// PSMT4
		case 0x14:
			
		// PSMT8H
		case 0x1b:
			
		// PSMT4HL
		case 0x24:
			
		// PSMT4HH
		case 0x2c:
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
		//case 0x35:
			
		// PSMZ16
		case 0x32:
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	
	// color lookup table pixel format variables //
	
	// assume clut bpp = 24/32, 32-bit ptr
	And3 = 0; Shift3 = 0; Shift4 = 0;
	
	if ( ( CLUTPixelFormat == 0x2 ) || ( CLUTPixelFormat == 0xa ) )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CLUT16BPP";
#endif

		// bpp = 16, ptr = 32-bit
		And3 = 1;
		Shift3 = 1;
		Shift4 = 4;
		
	}

	
	if ( !CLUTStoreMode )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CSM1";
#endif

		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
		
	}
	else
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CSM2";
#endif

		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}
	
	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	// important: reading from buffer in 32-bit units, so the texture buffer width should be divided accordingly
	//TexBufWidth >>= Shift1;
	
	// get the shade color
	//color_add = bgr;
	c1 = GPURegsGp.RGBAQ.R;
	c2 = GPURegsGp.RGBAQ.G;
	c3 = GPURegsGp.RGBAQ.B;
	ca = GPURegsGp.RGBAQ.A;

	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	x1 = xyz [ Coord1 ].X;
	y0 = xyz [ Coord0 ].Y;
	y1 = xyz [ Coord1 ].Y;
	
//cout << "\nrealds:" << dec << " c0=" << Coord0 << " c1=" << Coord1 << hex << " x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	

	// z0 should be same as z1 ??
	z0 = z1;

	// get fog coefficient
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	
	// get dzdx/dzdy for z
	// ***TODO*** this is incorrect since sometimes the x/y coords get switched. Need to fix
	if ( x1 - x0 )
	{
		//dzdx = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( x1 - x0 ) );
		dfdx = ( ( (s64)f1 - (s64)f0 ) << 28 ) / ( (s64) ( x1 - x0 ) );
	}
	
	if ( y1 - y0 )
	{
		//dzdy = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( y1 - y0 ) );
		dfdy = ( ( (s64)f1 - (s64)f0 ) << 28 ) / ( (s64) ( y1 - y0 ) );
	}
	
	// check if sprite should use UV coords or ST coords
	// on PS2, sprite can use the ST register to specify texture coords
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " FST";
#endif

		// get u,v
		u0 = uv [ Coord0 ].U;
		u1 = uv [ Coord1 ].U;
		v0 = uv [ Coord0 ].V;
		v1 = uv [ Coord1 ].V;
		
		// if using u,v coords, then the larger point should be minus one?
		//if ( u1 > u0 ) u1 -= ( 1 << 4 ); else u0 -= ( 1 << 4 );
		//if ( v1 > v0 ) v1 -= ( 1 << 4 ); else v0 -= ( 1 << 4 );
	}
	else
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " !FST";
#endif

		// get s,t
		// this appears to be correct
		//u0 = ( st [ Coord0 ].S >> 16 ) & 0x3fff;
		//u1 = ( st [ Coord1 ].S >> 16 ) & 0x3fff;
		//v0 = ( st [ Coord0 ].T >> 16 ) & 0x3fff;
		//v1 = ( st [ Coord1 ].T >> 16 ) & 0x3fff;
		
		// put s,t coords into 10.4 fixed point
		// note: tex width/height should probably be minus one
		u0 = (s32) ( st [ Coord0 ].fS * ( (float) ( TexWidth ) ) * 16.0f );
		u1 = (s32) ( st [ Coord1 ].fS * ( (float) ( TexWidth ) ) * 16.0f );
		v0 = (s32) ( st [ Coord0 ].fT * ( (float) ( TexHeight ) ) * 16.0f );
		v1 = (s32) ( st [ Coord1 ].fT * ( (float) ( TexHeight ) ) * 16.0f );
	}
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
	debug << dec << " u0=" << ( u0 >> 4 ) << " v0=" << ( v0 >> 4 ) << " u1=" << ( u1 >> 4 ) << " v1=" << ( v1 >> 4 );
	debug << hex << " z0=" << z0 << " z1=" << z1;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	// order coords so they go top to bottom and left to right
	if ( x1 < x0 )
	{
		// swap x,u coords
		Swap ( x0, x1 );
		Swap ( u0, u1 );
		
		// swap z,f coords ??
		// note: possibly need to update the z,f coords ??
		Swap ( z0, z1 );
		Swap ( f0, f1 );
	}
	
	if ( y1 < y0 )
	{
		// swap y,v coords
		Swap ( y0, y1 );
		Swap ( v0, v1 );
		
		// swap z,f coords ??
		// note: possibly need to update the z,f coords ??
		Swap ( z0, z1 );
		Swap ( f0, f1 );
	}

	
	// looks like some sprites have y1 < y0 and/or x1 < x0
	// didn't expect that so need to alert and figure out some other time
	if ( ( y1 < y0 ) || ( x1 < x0 ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: y1 < y0 or x1 < x0 in sprite!!!";
		debug << dec << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif

#ifdef VERBOSE_SPRITE_ERROR
		cout << "\nhps2x64: GPU: ERROR: Sprite has x1 < x0 or y1 < y0!!!";
#endif

		return;
	}
	
	
	// tex coords are in 10.4 fixed point
	//u0 <<= 12;
	//v0 <<= 12;
	//u1 <<= 12;
	//v1 <<= 12;
	u0 <<= 16;
	v0 <<= 16;
	u1 <<= 16;
	v1 <<= 16;
	
	if ( x1 - x0 ) dudx = ( u1 - u0 ) / ( x1 - x0 );
	if ( y1 - y0 ) dvdy = ( v1 - v0 ) / ( y1 - y0 );

	// shift back down to get into 16.16 format
	u0 >>= 4;
	v0 >>= 4;
	u1 >>= 4;
	v1 >>= 4;

	// the coords on the left and top are kept when equal (12.4 fixed point)
	//x0 += 0xf;
	//y0 += 0xf;
	
	// the coords on the right and bottom are not kept when equal (12.4 fixed point)
	//x1 -= 1;
	//y1 -= 1;
	
	// coords are in 12.4 fixed point
	StartX = ( x0 + 0xf ) >> 4;
	EndX = ( x1 - 1 ) >> 4;
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;

//cout << " sx=" << dec << StartX << " sy=" << StartY << " ex=" << EndX << " ey=" << EndY;
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
	debug << hex << " u0=" << u0 << " v0=" << v0 << " u1=" << u1 << " v1=" << v1;
#endif

	

	// check if sprite is within draw area
	if ( EndX < Window_XLeft || StartX > Window_XRight || EndY < Window_YTop || StartY > Window_YBottom ) return;
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawSprite" << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		debug << dec << " u0=" << ( u0 >> 16 ) << " v0=" << ( v0 >> 16 ) << " u1=" << ( u1 >> 16 ) << " v1=" << ( v1 >> 16 );
		debug << hex << " z0=" << z0 << " z1=" << z1;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << dec << "; TexBufWidth=" << TexBufWidth;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << dec << " FrameBufWidth=" << FrameBufferWidth_Pixels;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Shift1=" << hex << Shift1;
		debug << " And3=" << hex << And3;
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " FIX=" << (u32) ALPHA_X.FIX;
		debug << " A=" << (u32) uA;
		debug << " B=" << (u32) uB;
		debug << " C=" << (u32) uC;
		debug << " D=" << (u32) uD;
		debug << " c1=" << (u32) c1;
		debug << " c2=" << (u32) c2;
		debug << " c3=" << (u32) c3;
		debug << " ca=" << (u32) ca;
		debug << " TFX=" << (u32) TEXX [ Ctx ].TFX;
		debug << " TCC=" << (u32) TEXX [ Ctx ].TCC;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif



	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// can only have coords in range -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// can only have coords in range -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// can only have coords in range -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// can only have coords in range -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}


	
	//StartX = x0;
	//EndX = x1;
	//StartY = y0;
	//EndY = y1;
	
	// give z and f a fractional part
	//z0 <<= 23;
	f0 <<= 24;
	
	
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		//v0 += ( Window_YTop - StartY ) * dvdy;
		Temp += ( Window_YTop - StartY ) << 4;
		StartY = Window_YTop;
	}
	
	v0 += ( dvdy >> 4 ) * Temp;
	
	// also update z and f
	//z0 += ( dzdy >> 4 ) * Temp;
	f0 += ( dfdy >> 4 ) * Temp;
	
	if ( EndY > Window_YBottom )
	{
		EndY = Window_YBottom;
	}
	
	
	Temp = ( StartX << 4 ) - x0;
	
	if ( StartX < Window_XLeft )
	{
		//u0 += ( Window_XLeft - StartX ) * dudx;
		Temp += ( Window_XLeft - StartX ) << 4;
		StartX = Window_XLeft;
	}
	
	u0 += ( dudx >> 4 ) * Temp;
	
	// also update z and f
	//z0 += ( dzdx >> 4 ) * Temp;
	f0 += ( dfdx >> 4 ) * Temp;
	
	if ( EndX > Window_XRight )
	{
		EndX = Window_XRight;
	}
	

	// check that there is a pixel to draw
	if ( ( EndX < StartX ) || ( EndY < StartY ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: EndY < StartY or EndX < StartX in sprite!!!";
		debug << dec << " FinalCoords: StartX=" << StartX << " StartY=" << StartY << " EndX=" << EndX << " EndY=" << EndY;
#endif

		return;
	}
	
	
	
	
	NumberOfPixelsDrawn = ( EndX - StartX + 1 ) * ( EndY - StartY + 1 );
		
	// there are probably multiple pixel pipelines, so  might need to divide by like 8 or 16 or something
	// multiplying by 2 since the pixels has to be read before drawing
	//BusyUntil_Cycle = *_DebugCycleCount + ( NumberOfPixelsDrawn << 1 );
	if ( BusyUntil_Cycle < *_DebugCycleCount )
	{
		BusyUntil_Cycle = *_DebugCycleCount + ( ( NumberOfPixelsDrawn << 1 ) >> 4 );
	}
	//else
	//{
	//	BusyUntil_Cycle += ( NumberOfPixelsDrawn << 1 );
	//}

	
	//iV = v;
	iV = v0;
	

		for ( Line = StartY; Line <= EndY; Line++ )
		{
//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << "\r\n";
//#endif
//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << " y=" << dec << Line;
//#endif

	
			// need to start texture coord from left again
			//iU = u;
			iU = u0;
			
			//TexCoordY = (u8) ( ( iV & ~( TWH << 3 ) ) | ( ( TWY & TWH ) << 3 ) );
			//TexCoordY = (u8) ( ( iV & Not_TWH ) | ( TWYTWH ) );
			//TexCoordY <<= 10;
			TexCoordY = ( iV >> 16 ) /* & TexHeight_Mask */;
			
			TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
			TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
			TexCoordY &= TexY_And;
			TexCoordY |= TexY_Or;
			
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			//ptr32 = & ( buf [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			
//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << " TY=" << dec << TexCoordY;
//#endif

			// initialize z for line
			//iZ = z0;
			
			// initialize f for line
			iF = f0;

			// draw horizontal line
			//for ( x_across = StartX; x_across <= EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr32 < PtrEnd */; x_across += c_iVectorSize )
			{
//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << " x=" << dec << x_across;
//#endif

				// get pointer into frame buffer
				//ptr32 = & ( buf32 [ CvtAddrPix32 ( x_across, Line, FrameBufferWidth_Pixels ) ] );


				//TexCoordX = (u8) ( ( iU & Not_TWW ) | ( TWXTWW ) );
				TexCoordX = ( iU >> 16 ) /* & TexWidth_Mask */;
				
				TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				TexCoordX &= TexX_And;
				TexCoordX |= TexX_Or;


//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << " TX=" << dec << TexCoordX;
//#endif


//if ( !Line )
//{				
//	debug << "\r\nx_across=" << dec << x_across << " Offset=" << CvtAddrPix32 ( x_across, Line, FrameBufferWidth_Pixels ) << " TOffset=" << ;
//}
				
				// get texel from texture buffer (32-bit frame buffer) //
				
				//bgr = ptr_texture [ ( TexCoordX >> Shift1 ) + ( TexCoordY * ( TexBufWidth >> Shift1 ) ) ];
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT24
					case 1:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16
					case 2:
						bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16S
					case 0xa:
						bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT8
					case 0x13:
						bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT4
					case 0x14:
						bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						break;
						
					// PSMT8H
					case 0x1b:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						break;
						
					// PSMT4HL
					case 0x24:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16
					case 0x32:
						bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16S
					case 0x3a:
						bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
				}
				
				// look up color value in CLUT if needed //
				//if ( PixelFormat >> 4 )
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << " IDX=" << hex << bgr_temp;
//#endif

					bgr = ptr_clut16 [ bgr_temp ];
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
					}
				}


//debug << " BGRSRC=" << hex << bgr;

				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// pixel is probably not transparent ?? //
						
						bgr_temp = bgr;
						
						// convert to 32-bit
						bgr = ConvertPixel16To24 ( bgr );
						
						// set the texture alpha for 16 bit pixel
						bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					}
					/*
					else if ( bgr )
					{
						// set the texture alpha for 16 bit pixel
						bgr |= GPURegsGp.TEXA.TA0 << 24;
					}
					*/
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					bgr &= 0xffffff;
					
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// alpha is TA0
						bgr |= ( GPURegsGp.TEXA.TA0 << 24 );
					}
#ifdef ENABLE_TRANSPARENT24
					else
					{
						// if alpha is not TA0, then it is transparent??
						iU += dudx;
						
						//iZ += dzdx;
						iF += dfdx;
						continue;
					}
#endif
				}
				

//#if defined INLINE_DEBUG_SPRITE_PIXEL
//	debug << " BGR=" << hex << bgr;
//#endif

				// texture function ??
				
				
				// fog function ??
				
				
				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Texture ( x_across, Line, z1, bgr, c1, c2, c3, ca, iF >> 24 );
				
				// update z on line
				//iZ += dzdx;
				
				// update f on line
				iF += dfdx;
				
				/////////////////////////////////////////////////////////
				// interpolate texture coords across
				//iU += c_iVectorSize;
				iU += dudx;
				
				// update pointer for pixel out
				//ptr32 += c_iVectorSize;
					
			}
			
			/////////////////////////////////////////////////////////
			// interpolate texture coords down
			//iV++;
			iV += dvdy;
			
			//z0 += dzdy;
			
			f0 += dfdy;
		}
		
		
	
}


#ifdef _ENABLE_SSE41_SPRITE

void GPU::RenderSprite_DSx4 ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderSprite";
#endif

	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 4;


	
	u16 *CLUT_LUT;
	
	u32 TexelIndex;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u8 *ptr_texture8;
	u16 *ptr_texture16;
	u32 *ptr_texture32;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;

	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	s32 StartX, EndX, StartY, EndY;
	u32 NumberOfPixelsDrawn;
	
	
	u32 *ptr32;
	u16 *ptr16;
	
	u32 *zptr32;
	u16 *zptr16;
	
	u32 DestPixel, bgr, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	s32 u0, v0, u1, v1;

	// and the z coords
	s64 z0, z1;
	
	// interpolate the z ??
	s64 dzdx, dzdy;
	s64 iZ;
	
	// interpolate f for fog
	s64 f0, f1;
	s64 dfdx, dfdy;
	s64 iF;
	
	
	s32 iU, iV, dudx, dvdy;
	
	s32 Line, x_across;
	
	u32 c1, c2, c3, ca;
	
	s64 Temp;
	
	
	s32 TexCoordX, TexCoordY;
	
	u32 TextureOffset;
	
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	

	__m128i vEnable;
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 ( 3, 2, 1, 0 );
	
	__m128i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m128i vXInc;
	__m128i vYInc;
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vYInc = _mm_set1_epi32 ( 1 );

	__m128i vTexOffset32, vUinc, vVinc, vUidentity, vUstart, vUacross, vVacross, vFacross, vTexBufWidth;
	__m128i vTexCoordY, vTexY_Min, vTexY_Max, vTexY_And, vTexY_Or;
	__m128i vTexCoordX, vTexX_Min, vTexX_Max, vTexX_And, vTexX_Or;
	__m128i vRed, vGreen, vBlue, vAlpha;
	__m128i vBgr_temp, vBgr2, vca, vc1, vc2, vc3;
	
	// for testing
	u32 Exp0, Exp1, Exp2, Exp3;
	u32 Res0, Res1, Res2, Res3;

	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	

	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;

	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	//TextureOffset = ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 );
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	// this is the offset / 16
	// note: this is actually the position in the temporary buffer, not in the local memory
	CLUTOffset = TEX0 [ Ctx ].CSA;

	

	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif

	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	// *** TODO *** use a union
	ptr_texture8 = (u8*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture32 = (u32*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	// texture pixel format variables //
	
	
	
	
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	


	
	// color lookup table pixel format variables //
	

	
	if ( !CLUTStoreMode )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CSM1";
#endif

		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
		
	}
	else
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CSM2";
#endif

	}
	
	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	// get the shade color
	c1 = GPURegsGp.RGBAQ.R;
	c2 = GPURegsGp.RGBAQ.G;
	c3 = GPURegsGp.RGBAQ.B;
	ca = GPURegsGp.RGBAQ.A;

	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	x1 = xyz [ Coord1 ].X;
	y0 = xyz [ Coord0 ].Y;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	

	// z0 should be same as z1 ??
	z0 = z1;

	// get fog coefficient
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	
	// get dzdx/dzdy for z
	// ***TODO*** this is incorrect since sometimes the x/y coords get switched. Need to fix
	if ( x1 - x0 )
	{
		//dzdx = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( x1 - x0 ) );
		dfdx = ( ( (s64)f1 - (s64)f0 ) << 28 ) / ( (s64) ( x1 - x0 ) );
	}
	
	if ( y1 - y0 )
	{
		//dzdy = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( y1 - y0 ) );
		dfdy = ( ( (s64)f1 - (s64)f0 ) << 28 ) / ( (s64) ( y1 - y0 ) );
	}
	
	// check if sprite should use UV coords or ST coords
	// on PS2, sprite can use the ST register to specify texture coords
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " FST";
#endif

		// get u,v
		u0 = uv [ Coord0 ].U;
		u1 = uv [ Coord1 ].U;
		v0 = uv [ Coord0 ].V;
		v1 = uv [ Coord1 ].V;
		
	}
	else
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " !FST";
#endif

		
		// put s,t coords into 10.4 fixed point
		// note: tex width/height should probably be minus one
		u0 = (s32) ( st [ Coord0 ].fS * ( (float) ( TexWidth ) ) * 16.0f );
		u1 = (s32) ( st [ Coord1 ].fS * ( (float) ( TexWidth ) ) * 16.0f );
		v0 = (s32) ( st [ Coord0 ].fT * ( (float) ( TexHeight ) ) * 16.0f );
		v1 = (s32) ( st [ Coord1 ].fT * ( (float) ( TexHeight ) ) * 16.0f );
	}
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
	debug << dec << " u0=" << ( u0 >> 4 ) << " v0=" << ( v0 >> 4 ) << " u1=" << ( u1 >> 4 ) << " v1=" << ( v1 >> 4 );
	debug << hex << " z0=" << z0 << " z1=" << z1;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	// order coords so they go top to bottom and left to right
	if ( x1 < x0 )
	{
		// swap x,u coords
		Swap ( x0, x1 );
		Swap ( u0, u1 );
		
		// swap z,f coords ??
		// note: possibly need to update the z,f coords ??
		//Swap ( z0, z1 );
		//Swap ( f0, f1 );
	}
	
	if ( y1 < y0 )
	{
		// swap y,v coords
		Swap ( y0, y1 );
		Swap ( v0, v1 );
		
		// swap z,f coords ??
		// note: possibly need to update the z,f coords ??
		//Swap ( z0, z1 );
		//Swap ( f0, f1 );
	}

	
	// looks like some sprites have y1 < y0 and/or x1 < x0
	// didn't expect that so need to alert and figure out some other time
	if ( ( y1 < y0 ) || ( x1 < x0 ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: y1 < y0 or x1 < x0 in sprite!!!";
		debug << dec << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif

#ifdef VERBOSE_SPRITE_ERROR
		cout << "\nhps2x64: GPU: ERROR: Sprite has x1 < x0 or y1 < y0!!!";
#endif

		return;
	}
	
	
	// tex coords are in 10.4 fixed point
	u0 <<= 16;
	v0 <<= 16;
	u1 <<= 16;
	v1 <<= 16;
	
	if ( x1 - x0 ) dudx = ( u1 - u0 ) / ( x1 - x0 );
	if ( y1 - y0 ) dvdy = ( v1 - v0 ) / ( y1 - y0 );

	// shift back down to get into 16.16 format
	u0 >>= 4;
	v0 >>= 4;
	u1 >>= 4;
	v1 >>= 4;

	
	// coords are in 12.4 fixed point
	StartX = ( x0 + 0xf ) >> 4;
	EndX = ( x1 - 1 ) >> 4;
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
	debug << hex << " u0=" << u0 << " v0=" << v0 << " u1=" << u1 << " v1=" << v1;
#endif

	

	// check if sprite is within draw area
	if ( EndX < Window_XLeft || StartX > Window_XRight || EndY < Window_YTop || StartY > Window_YBottom ) return;
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawSprite" << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		debug << dec << " u0=" << ( u0 >> 16 ) << " v0=" << ( v0 >> 16 ) << " u1=" << ( u1 >> 16 ) << " v1=" << ( v1 >> 16 );
		debug << hex << " z0=" << z0 << " z1=" << z1;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << dec << "; TexBufWidth=" << TexBufWidth;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << dec << " FrameBufWidth=" << FrameBufferWidth_Pixels;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif



	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// can only have coords in range -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// can only have coords in range -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// can only have coords in range -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// can only have coords in range -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}


	
	
	// give z and f a fractional part
	//z0 <<= 23;
	f0 <<= 24;
	
	
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		Temp += ( Window_YTop - StartY ) << 4;
		StartY = Window_YTop;
	}
	
	v0 += ( dvdy >> 4 ) * Temp;
	
	// also update z and f
	//z0 += ( dzdy >> 4 ) * Temp;
	f0 += ( dfdy >> 4 ) * Temp;
	
	if ( EndY > Window_YBottom )
	{
		EndY = Window_YBottom;
	}
	
	
	Temp = ( StartX << 4 ) - x0;
	
	if ( StartX < Window_XLeft )
	{
		Temp += ( Window_XLeft - StartX ) << 4;
		StartX = Window_XLeft;
	}
	
	u0 += ( dudx >> 4 ) * Temp;
	
	// also update z and f
	//z0 += ( dzdx >> 4 ) * Temp;
	f0 += ( dfdx >> 4 ) * Temp;
	
	if ( EndX > Window_XRight )
	{
		EndX = Window_XRight;
	}
	

	// check that there is a pixel to draw
	if ( ( EndX < StartX ) || ( EndY < StartY ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: EndY < StartY or EndX < StartX in sprite!!!";
		debug << dec << " FinalCoords: StartX=" << StartX << " StartY=" << StartY << " EndX=" << EndX << " EndY=" << EndY;
#endif

		return;
	}
	
	
	
	
	NumberOfPixelsDrawn = ( EndX - StartX + 1 ) * ( EndY - StartY + 1 );
		
	// there are probably multiple pixel pipelines, so  might need to divide by like 8 or 16 or something
	// multiplying by 2 since the pixels has to be read before drawing
	//BusyUntil_Cycle = *_DebugCycleCount + ( NumberOfPixelsDrawn << 1 );
	if ( BusyUntil_Cycle < *_DebugCycleCount )
	{
		BusyUntil_Cycle = *_DebugCycleCount + ( ( NumberOfPixelsDrawn << 1 ) >> 4 );
	}

	
	iV = v0;


	//vBgr = _mm_set1_epi32 ( bgr );
	vStartX = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
	vYacross = _mm_set1_epi32 ( StartY );
	vZacross = _mm_set1_epi32 ( z1 );
	vXmax = _mm_set1_epi32 ( EndX + 1 );
	
	vAlphaSelect [ 0 ] = _mm_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm_set1_epi32 ( AlphaSelect [ 3 ] );

	
	vUinc = _mm_set1_epi32 ( dudx * c_iVectorSize );
	vVinc = _mm_set1_epi32 ( dvdy );
	
	vUidentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dudx ), vIdentity );
	
	vUstart = _mm_add_epi32 ( _mm_set1_epi32 ( u0 ), vUidentity );
	vVacross = _mm_set1_epi32 ( v0 );

	vFacross = _mm_set1_epi32 ( f1 );
	vTexBufWidth = _mm_set1_epi32 ( TexBufWidth );
	
	vc1 = _mm_set1_epi32 ( c1 );
	vc2 = _mm_set1_epi32 ( c2 );
	vc3 = _mm_set1_epi32 ( c3 );
	vca = _mm_set1_epi32 ( ca );
	
	vTexY_Min = _mm_set1_epi32 ( TexY_Min );
	vTexY_Max = _mm_set1_epi32 ( TexY_Max );
	vTexY_And = _mm_set1_epi32 ( TexY_And );
	vTexY_Or = _mm_set1_epi32 ( TexY_Or );
	
	vTexX_Min = _mm_set1_epi32 ( TexX_Min );
	vTexX_Max = _mm_set1_epi32 ( TexX_Max );
	vTexX_And = _mm_set1_epi32 ( TexX_And );
	vTexX_Or = _mm_set1_epi32 ( TexX_Or );

		for ( Line = StartY; Line <= EndY; Line++ )
		{
			vXacross = vStartX;
	
			// need to start texture coord from left again
			//iU = u0;
			vUacross = vUstart;
			
			//TexCoordY = ( iV >> 16 );
			vTexCoordY = _mm_srai_epi32 ( vVacross, 16 );
			
			//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
			//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
			//TexCoordY &= TexY_And;
			//TexCoordY |= TexY_Or;
			vTexCoordY = _mm_max_epi32 ( vTexCoordY, vTexY_Min );
			vTexCoordY = _mm_min_epi32 ( vTexCoordY, vTexY_Max );
			vTexCoordY = _mm_and_si128 ( vTexCoordY, vTexY_And );
			vTexCoordY = _mm_or_si128 ( vTexCoordY, vTexY_Or );
			

			// initialize z for line
			//iZ = z0;
			
			// initialize f for line
			//iF = f0;

			// draw horizontal line
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				vTexCoordX = _mm_srai_epi32 ( vUacross, 16 );
				
				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;
				vTexCoordX = _mm_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm_and_si128 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm_or_si128 ( vTexCoordX, vTexX_Or );

				
				// get texel from texture buffer (32-bit frame buffer) //
				
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 24 ), 24 );
#else
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 28 ), 28 );
#else
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> _mm_extract_epi32 ( vBgr, 3 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> _mm_extract_epi32 ( vBgr, 2 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> _mm_extract_epi32 ( vBgr, 1 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> _mm_extract_epi32 ( vBgr, 0 ) ) & 0xf );
#endif
						
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 24 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 );
#endif
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 4 ), 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 ) & 0xf );
#endif
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 28 );
#endif
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
#ifdef ENABLE_AVX128
					vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
					//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_clut16, vBgr_temp, 2 );
#else
					//bgr = ptr_clut16 [ bgr_temp ];
					vBgr = _mm_set_epi32 (
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) ] );
#endif
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
#ifdef ENABLE_AVX128
						vBgr2 = _mm_mask_i32gather_epi32 ( vBgr2, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						//vBgr2 = _mm_i32gather_epi32 ( (int const*) ( ptr_clut16 + 256 ), vBgr_temp, 2 );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#else
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
						vBgr2 = _mm_set_epi32 (
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) + 256 ] );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#endif
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					//vAlpha = _mm_slli_epi32 ( _mm_srli_epi32 ( vBgr, 15 ), 31 );
					//vBgr = _mm_or_si128 ( _mm_or_si128 ( _mm_or_si128 ( vRed, vAlpha ), _mm_slli_epi32 ( vGreen, 8 + 3 ) ), _mm_slli_epi32 ( vBlue, 16 + 3 ) );
					vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 + 3 ) ), _mm_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm_or_si128 ( vBgr, _mm_blendv_epi8 ( vTA0, vTA1, _mm_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), vBgr );
						//vBgr = _mm_and_si128 ( _mm_xor_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), _mm_set1_epi32 ( -1 ) ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm_or_si128 ( vBgr, vTA0 );
					
					
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_set1_epi32 ( 0 ) ), vBgr );
						//vBgr = _mm_and_si128 ( _mm_xor_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), _mm_set1_epi32 ( -1 ) ), vBgr );

						
					}
					
				}
				

				// also need to make zero 32-bit pixels transparent
				/*
				if ( ( !PixelFormat ) || ( ( PixelFormat > 0xa ) && ( !CLUTPixelFormat ) ) )
				{
					//vEnable = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( _mm_srli_epi32 ( vBgr, 24 ), _mm_setzero_si128 () ), vEnable );
					//vEnable = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr, _mm_setzero_si128 () ), vEnable );
					vEnable = _mm_and_si128 ( _mm_xor_si128 ( _mm_cmpeq_epi32 ( vBgr, _mm_setzero_si128 () ), _mm_set1_epi32 ( -1 ) ), vEnable );
				}
				*/
				

				
				// texture function ??
				vBgr = TextureFunc32x4 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vBgr = FogFunc32x4 ( vBgr, vFacross );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;

/*
if ( _mm_extract_epi32 ( vEnable, 0 ) )
{
debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 0 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 0 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 0 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 0 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 0 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 0 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 0 );
}
*/

				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, z1, bgr, c1, c2, c3, ca, iF >> 24 );
				//PlotPixel_Texture ( x_across, Line, z1, bgr, c1, c2, c3, ca, f1 );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
				
				/*
				AlphaSelect [ 0 ] = _mm_extract_epi32 ( vAlphaSelect [ 0 ], 0 );
				if ( _mm_extract_epi32 ( vEnable, 0 ) ) PlotPixel_Mono ( _mm_extract_epi32 ( vXacross, 0 ), _mm_extract_epi32 ( vYacross, 0 ), z1, _mm_extract_epi32 ( vBgr, 0 ) );
				AlphaSelect [ 0 ] = _mm_extract_epi32 ( vAlphaSelect [ 0 ], 1 );
				if ( _mm_extract_epi32 ( vEnable, 1 ) ) PlotPixel_Mono ( _mm_extract_epi32 ( vXacross, 1 ), _mm_extract_epi32 ( vYacross, 1 ), z1, _mm_extract_epi32 ( vBgr, 1 ) );
				AlphaSelect [ 0 ] = _mm_extract_epi32 ( vAlphaSelect [ 0 ], 2 );
				if ( _mm_extract_epi32 ( vEnable, 2 ) ) PlotPixel_Mono ( _mm_extract_epi32 ( vXacross, 2 ), _mm_extract_epi32 ( vYacross, 2 ), z1, _mm_extract_epi32 ( vBgr, 2 ) );
				AlphaSelect [ 0 ] = _mm_extract_epi32 ( vAlphaSelect [ 0 ], 3 );
				if ( _mm_extract_epi32 ( vEnable, 3 ) ) PlotPixel_Mono ( _mm_extract_epi32 ( vXacross, 3 ), _mm_extract_epi32 ( vYacross, 3 ), z1, _mm_extract_epi32 ( vBgr, 3 ) );
				*/
				
				/*
				if ( _mm_extract_epi32 ( vEnable, 0 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 0 ), _mm_extract_epi32 ( vYacross, 0 ), z1, _mm_extract_epi32 ( vBgr, 0 ), c1, c2, c3, ca, f1 );
				if ( _mm_extract_epi32 ( vEnable, 1 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 1 ), _mm_extract_epi32 ( vYacross, 1 ), z1, _mm_extract_epi32 ( vBgr, 1 ), c1, c2, c3, ca, f1 );
				if ( _mm_extract_epi32 ( vEnable, 2 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 2 ), _mm_extract_epi32 ( vYacross, 2 ), z1, _mm_extract_epi32 ( vBgr, 2 ), c1, c2, c3, ca, f1 );
				if ( _mm_extract_epi32 ( vEnable, 3 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 3 ), _mm_extract_epi32 ( vYacross, 3 ), z1, _mm_extract_epi32 ( vBgr, 3 ), c1, c2, c3, ca, f1 );
				*/
				
				
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				// update z on line
				//iZ += dzdx;
				
				// update f on line
				//iF += dfdx;
				
				/////////////////////////////////////////////////////////
				// interpolate texture coords across
				//iU += dudx;
				vUacross = _mm_add_epi32 ( vUacross, vUinc );
					
			}
			
			vYacross = _mm_add_epi32 ( vYacross, vYInc );
			
			/////////////////////////////////////////////////////////
			// interpolate texture coords down
			iV += dvdy;
			vVacross = _mm_add_epi32 ( vVacross, vVinc );
			
			//z0 += dzdy;
			
			//f0 += dfdy;
		}
		
		
	
}

#endif

#ifdef _ENABLE_AVX2_SPRITE

void GPU::RenderSprite_DSx8 ( u32 Coord0, u32 Coord1 )
{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; RenderSprite";
#endif

	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 8;


	
	u16 *CLUT_LUT;
	
	u32 TexelIndex;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u8 *ptr_texture8;
	u16 *ptr_texture16;
	u32 *ptr_texture32;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;

	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	s32 StartX, EndX, StartY, EndY;
	u32 NumberOfPixelsDrawn;
	
	
	u32 *ptr32;
	u16 *ptr16;
	
	u32 *zptr32;
	u16 *zptr16;
	
	u32 DestPixel, bgr, bgr_temp;
	
	// 12.4 fixed point
	s32 x0, y0, x1, y1;
	s32 u0, v0, u1, v1;

	// and the z coords
	s64 z0, z1;
	
	// interpolate the z ??
	s64 dzdx, dzdy;
	s64 iZ;
	
	// interpolate f for fog
	s64 f0, f1;
	s64 dfdx, dfdy;
	s64 iF;
	
	
	s32 iU, iV, dudx, dvdy;
	
	s32 Line, x_across;
	
	u32 c1, c2, c3, ca;
	
	s64 Temp;
	
	
	s32 TexCoordX, TexCoordY;
	
	u32 TextureOffset;
	
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	

	__m256i vEnable;
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 ( 7, 6, 5, 4, 3, 2, 1, 0 );
	
	__m256i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m256i vXInc;
	__m256i vYInc;
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vYInc = _mm256_set1_epi32 ( 1 );

	__m256i vTexOffset32, vUinc, vVinc, vUidentity, vUstart, vUacross, vVacross, vFacross, vTexBufWidth;
	__m256i vTexCoordY, vTexY_Min, vTexY_Max, vTexY_And, vTexY_Or;
	__m256i vTexCoordX, vTexX_Min, vTexX_Max, vTexX_And, vTexX_Or;
	__m256i vRed, vGreen, vBlue, vAlpha;
	__m256i vBgr_temp, vBgr2, vca, vc1, vc2, vc3;
	
	// for testing
	u32 Exp0, Exp1, Exp2, Exp3;
	u32 Res0, Res1, Res2, Res3;

	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	

	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;

	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	//TextureOffset = ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 );
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	// this is the offset / 16
	// note: this is actually the position in the temporary buffer, not in the local memory
	CLUTOffset = TEX0 [ Ctx ].CSA;

	

	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif

	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	// *** TODO *** use a union
	ptr_texture8 = (u8*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture32 = (u32*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	// texture pixel format variables //
	
	
	
	
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	


	
	// color lookup table pixel format variables //
	

	
	if ( !CLUTStoreMode )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CSM1";
#endif

		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
		
	}
	else
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " CSM2";
#endif

	}
	
	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	// get the shade color
	c1 = GPURegsGp.RGBAQ.R;
	c2 = GPURegsGp.RGBAQ.G;
	c3 = GPURegsGp.RGBAQ.B;
	ca = GPURegsGp.RGBAQ.A;

	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	x1 = xyz [ Coord1 ].X;
	y0 = xyz [ Coord0 ].Y;
	y1 = xyz [ Coord1 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	

	// z0 should be same as z1 ??
	z0 = z1;

	// get fog coefficient
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	
	// get dzdx/dzdy for z
	// ***TODO*** this is incorrect since sometimes the x/y coords get switched. Need to fix
	if ( x1 - x0 )
	{
		//dzdx = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( x1 - x0 ) );
		dfdx = ( ( (s64)f1 - (s64)f0 ) << 28 ) / ( (s64) ( x1 - x0 ) );
	}
	
	if ( y1 - y0 )
	{
		//dzdy = ( ( (s64)z1 - (s64)z0 ) << 27 ) / ( (s64) ( y1 - y0 ) );
		dfdy = ( ( (s64)f1 - (s64)f0 ) << 28 ) / ( (s64) ( y1 - y0 ) );
	}
	
	// check if sprite should use UV coords or ST coords
	// on PS2, sprite can use the ST register to specify texture coords
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " FST";
#endif

		// get u,v
		u0 = uv [ Coord0 ].U;
		u1 = uv [ Coord1 ].U;
		v0 = uv [ Coord0 ].V;
		v1 = uv [ Coord1 ].V;
		
	}
	else
	{
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << " !FST";
#endif

		
		// put s,t coords into 10.4 fixed point
		// note: tex width/height should probably be minus one
		u0 = (s32) ( st [ Coord0 ].fS * ( (float) ( TexWidth ) ) * 16.0f );
		u1 = (s32) ( st [ Coord1 ].fS * ( (float) ( TexWidth ) ) * 16.0f );
		v0 = (s32) ( st [ Coord0 ].fT * ( (float) ( TexHeight ) ) * 16.0f );
		v1 = (s32) ( st [ Coord1 ].fT * ( (float) ( TexHeight ) ) * 16.0f );
	}
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
	debug << dec << " u0=" << ( u0 >> 4 ) << " v0=" << ( v0 >> 4 ) << " u1=" << ( u1 >> 4 ) << " v1=" << ( v1 >> 4 );
	debug << hex << " z0=" << z0 << " z1=" << z1;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	
	// order coords so they go top to bottom and left to right
	if ( x1 < x0 )
	{
		// swap x,u coords
		Swap ( x0, x1 );
		Swap ( u0, u1 );
		
		// swap z,f coords ??
		// note: possibly need to update the z,f coords ??
		//Swap ( z0, z1 );
		//Swap ( f0, f1 );
	}
	
	if ( y1 < y0 )
	{
		// swap y,v coords
		Swap ( y0, y1 );
		Swap ( v0, v1 );
		
		// swap z,f coords ??
		// note: possibly need to update the z,f coords ??
		//Swap ( z0, z1 );
		//Swap ( f0, f1 );
	}

	
	// looks like some sprites have y1 < y0 and/or x1 < x0
	// didn't expect that so need to alert and figure out some other time
	if ( ( y1 < y0 ) || ( x1 < x0 ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: y1 < y0 or x1 < x0 in sprite!!!";
		debug << dec << " FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
#endif

#ifdef VERBOSE_SPRITE_ERROR
		cout << "\nhps2x64: GPU: ERROR: Sprite has x1 < x0 or y1 < y0!!!";
#endif

		return;
	}
	
	
	// tex coords are in 10.4 fixed point
	u0 <<= 16;
	v0 <<= 16;
	u1 <<= 16;
	v1 <<= 16;
	
	if ( x1 - x0 ) dudx = ( u1 - u0 ) / ( x1 - x0 );
	if ( y1 - y0 ) dvdy = ( v1 - v0 ) / ( y1 - y0 );

	// shift back down to get into 16.16 format
	u0 >>= 4;
	v0 >>= 4;
	u1 >>= 4;
	v1 >>= 4;

	
	// coords are in 12.4 fixed point
	StartX = ( x0 + 0xf ) >> 4;
	EndX = ( x1 - 1 ) >> 4;
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	
#if defined INLINE_DEBUG_SPRITE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; FinalCoords: x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1;
	debug << hex << " u0=" << u0 << " v0=" << v0 << " u1=" << u1 << " v1=" << v1;
#endif

	

	// check if sprite is within draw area
	if ( EndX < Window_XLeft || StartX > Window_XRight || EndY < Window_YTop || StartY > Window_YBottom ) return;
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawSprite" << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
		debug << dec << " u0=" << ( u0 >> 16 ) << " v0=" << ( v0 >> 16 ) << " u1=" << ( u1 >> 16 ) << " v1=" << ( v1 >> 16 );
		debug << hex << " z0=" << z0 << " z1=" << z1;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << dec << "; TexBufWidth=" << TexBufWidth;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << dec << " FrameBufWidth=" << FrameBufferWidth_Pixels;
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif



	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// can only have coords in range -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// can only have coords in range -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// can only have coords in range -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// can only have coords in range -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}


	
	
	// give z and f a fractional part
	//z0 <<= 23;
	f0 <<= 24;
	
	
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		Temp += ( Window_YTop - StartY ) << 4;
		StartY = Window_YTop;
	}
	
	v0 += ( dvdy >> 4 ) * Temp;
	
	// also update z and f
	//z0 += ( dzdy >> 4 ) * Temp;
	f0 += ( dfdy >> 4 ) * Temp;
	
	if ( EndY > Window_YBottom )
	{
		EndY = Window_YBottom;
	}
	
	
	Temp = ( StartX << 4 ) - x0;
	
	if ( StartX < Window_XLeft )
	{
		Temp += ( Window_XLeft - StartX ) << 4;
		StartX = Window_XLeft;
	}
	
	u0 += ( dudx >> 4 ) * Temp;
	
	// also update z and f
	//z0 += ( dzdx >> 4 ) * Temp;
	f0 += ( dfdx >> 4 ) * Temp;
	
	if ( EndX > Window_XRight )
	{
		EndX = Window_XRight;
	}
	

	// check that there is a pixel to draw
	if ( ( EndX < StartX ) || ( EndY < StartY ) )
	{
#if defined INLINE_DEBUG_SPRITE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << "\r\nERROR: EndY < StartY or EndX < StartX in sprite!!!";
		debug << dec << " FinalCoords: StartX=" << StartX << " StartY=" << StartY << " EndX=" << EndX << " EndY=" << EndY;
#endif

		return;
	}
	
	
	
	
	NumberOfPixelsDrawn = ( EndX - StartX + 1 ) * ( EndY - StartY + 1 );
		
	// there are probably multiple pixel pipelines, so  might need to divide by like 8 or 16 or something
	// multiplying by 2 since the pixels has to be read before drawing
	//BusyUntil_Cycle = *_DebugCycleCount + ( NumberOfPixelsDrawn << 1 );
	if ( BusyUntil_Cycle < *_DebugCycleCount )
	{
		BusyUntil_Cycle = *_DebugCycleCount + ( ( NumberOfPixelsDrawn << 1 ) >> 4 );
	}

	
	iV = v0;


	//vBgr = _mm256_set1_epi32 ( bgr );
	vStartX = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
	vYacross = _mm256_set1_epi32 ( StartY );
	vZacross = _mm256_set1_epi32 ( z1 );
	vXmax = _mm256_set1_epi32 ( EndX + 1 );
	
	vAlphaSelect [ 0 ] = _mm256_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm256_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm256_set1_epi32 ( AlphaSelect [ 3 ] );

	
	vUinc = _mm256_set1_epi32 ( dudx * c_iVectorSize );
	vVinc = _mm256_set1_epi32 ( dvdy );
	
	vUidentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dudx ), vIdentity );
	
	vUstart = _mm256_add_epi32 ( _mm256_set1_epi32 ( u0 ), vUidentity );
	vVacross = _mm256_set1_epi32 ( v0 );

	vFacross = _mm256_set1_epi32 ( f1 );
	vTexBufWidth = _mm256_set1_epi32 ( TexBufWidth );
	
	vc1 = _mm256_set1_epi32 ( c1 );
	vc2 = _mm256_set1_epi32 ( c2 );
	vc3 = _mm256_set1_epi32 ( c3 );
	vca = _mm256_set1_epi32 ( ca );
	
	vTexY_Min = _mm256_set1_epi32 ( TexY_Min );
	vTexY_Max = _mm256_set1_epi32 ( TexY_Max );
	vTexY_And = _mm256_set1_epi32 ( TexY_And );
	vTexY_Or = _mm256_set1_epi32 ( TexY_Or );
	
	vTexX_Min = _mm256_set1_epi32 ( TexX_Min );
	vTexX_Max = _mm256_set1_epi32 ( TexX_Max );
	vTexX_And = _mm256_set1_epi32 ( TexX_And );
	vTexX_Or = _mm256_set1_epi32 ( TexX_Or );

		for ( Line = StartY; Line <= EndY; Line++ )
		{
			vXacross = vStartX;
	
			// need to start texture coord from left again
			//iU = u0;
			vUacross = vUstart;
			
			//TexCoordY = ( iV >> 16 );
			vTexCoordY = _mm256_srai_epi32 ( vVacross, 16 );
			
			//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
			//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
			//TexCoordY &= TexY_And;
			//TexCoordY |= TexY_Or;
			vTexCoordY = _mm256_max_epi32 ( vTexCoordY, vTexY_Min );
			vTexCoordY = _mm256_min_epi32 ( vTexCoordY, vTexY_Max );
			vTexCoordY = _mm256_and_si256 ( vTexCoordY, vTexY_And );
			vTexCoordY = _mm256_or_si256 ( vTexCoordY, vTexY_Or );
			

			// initialize z for line
			//iZ = z0;
			
			// initialize f for line
			//iF = f0;

			// draw horizontal line
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				vTexCoordX = _mm256_srai_epi32 ( vUacross, 16 );
				
				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;
				vTexCoordX = _mm256_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm256_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm256_and_si256 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm256_or_si256 ( vTexCoordX, vTexX_Or );

				
				// get texel from texture buffer (32-bit frame buffer) //
				
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix8x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 24 ), 24 );
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						vTexOffset32 = CvtAddrPix4x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm256_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 28 ), 28 );
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 24 );
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 4 ), 28 );
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 28 );
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					

					//bgr = ptr_clut16 [ bgr_temp ];
					
					vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
						
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
								
						vBgr2 = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						vBgr = _mm256_or_si256 ( vBgr, _mm256_slli_epi32 ( vBgr2, 16 ) );
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					//vAlpha = _mm256_slli_epi32 ( _mm256_srli_epi32 ( vBgr, 15 ), 31 );
					//vBgr = _mm256_or_si256 ( _mm256_or_si256 ( _mm256_or_si256 ( vRed, vAlpha ), _mm256_slli_epi32 ( vGreen, 8 + 3 ) ), _mm256_slli_epi32 ( vBlue, 16 + 3 ) );
					vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 + 3 ) ), _mm256_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm256_or_si256 ( vBgr, _mm256_blendv_epi8 ( vTA0, vTA1, _mm256_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), vBgr );
						//vBgr = _mm256_and_si256 ( _mm256_xor_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), _mm256_set1_epi32 ( -1 ) ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm256_or_si256 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_set1_epi32 ( 0 ) ), vBgr );
						//vBgr = _mm256_and_si256 ( _mm256_xor_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), _mm256_set1_epi32 ( -1 ) ), vBgr );

					}
					
				}
				

				

				
				// texture function ??
				vBgr = TextureFunc32x8 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vBgr = FogFunc32x8 ( vBgr, vFacross );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;


				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, z1, bgr, c1, c2, c3, ca, iF >> 24 );
				//PlotPixel_Texture ( x_across, Line, z1, bgr, c1, c2, c3, ca, f1 );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
				
				
				
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				// update z on line
				//iZ += dzdx;
				
				// update f on line
				//iF += dfdx;
				
				/////////////////////////////////////////////////////////
				// interpolate texture coords across
				iU += dudx;
				vUacross = _mm256_add_epi32 ( vUacross, vUinc );
					
			}
			
			vYacross = _mm256_add_epi32 ( vYacross, vYInc );
			
			/////////////////////////////////////////////////////////
			// interpolate texture coords down
			iV += dvdy;
			vVacross = _mm256_add_epi32 ( vVacross, vVinc );
			
			//z0 += dzdy;
			
			//f0 += dfdy;
		}
		
		
	
}

#endif



// shading
// texture mapping
// fogging
// alpha blending
// anti-aliasing
// texture coordinate spec
// alpha test enable
// destination alpha test enable
// zbuf test enable
// frame buffer pixel format (3-bits)
// frame buffer mask enable
// zbuf pixel format (2-bits)
// zbuf write enable
void GPU::DrawTriangle_Mono32_DS ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 1;

	//u32 Pixel, TexelIndex, Y1_OnLeft;
	//u32 color_add;
	
	//u32 Y1_OnLeft;
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;
	
	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	/*
	u32 *VRAM32;
	u16 *VRAM16;
	
	u32 *buf32;
	u16 *buf16;
	*/
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;
	//s64 Error_Left;	
	
	u32 DestPixel;
	
	
	/*
	u32 PixelMask = 0, SetPixelMask = 0;

	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];
	
	u32 PixelAlpha, PixelAlpha16;

	
	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	
	// z-buffer
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;


	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;
	
	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	
	//if ( GPU_CTRL_Read.ME ) PixelMask = 0x10000;
	//if ( GPU_CTRL_Read.MD ) SetPixelMask = 0x10000;

	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;

	VRAM32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	VRAM16 = (u16*) VRAM32;

	buf32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	buf16 = (u16*) buf32;
	*/
	
	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	// get the color for the triangle
	// ***TODO*** rgbaq_Current is just the value of the RGBAQ register, so this should be fixed so rgbaq_Current variable is not needed
	//bgr = rgbaq [ Coord0 ].Value & 0xffffffffULL;
	bgr = rgbaq_Current.Value & 0xffffffffULL;
	
	
	if ( FrameBuffer_PixelFormat > 1 )
	{
		// 16-bit frame buffer //
		
		// convert 32-bit pixel to 16-bit ahead of time
		bgr16 = Color24To16 ( bgr );
		
		// also bring the alpha into bgr16
		bgr16 |= ( bgr >> 16 ) & 0x8000;
		
		// add alpha value to 16-bit pixel
		// but it should probably be the source alpha value?
		//bgr16 |= ( ( ( bgr16 & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
		bgr16 |= ( bgr & 0xff000000 );
		
		bgr = bgr16;
	}
	
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	//if ( y1 < y0 )
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		//Swap ( x0, x1 );
		//Swap ( y0, y1 );
		Swap ( Coord0, Coord1 );
	}
	
	//if ( y2 < y0 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		//Swap ( x0, x2 );
		//Swap ( y0, y2 );
		Swap ( Coord2, Coord0 );
	}
	
	///////////////////////////////////////
	// put middle coordinates in x1,y1
	//if ( y2 < y1 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		//Swap ( x1, x2 );
		//Swap ( y1, y2 );
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// invert z for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
	z2 ^= 0xffffffffULL;
#endif
	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	// ***todo*** this is wrong
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleMono" << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		debug << " Alpha=" << Alpha;
		debug << " FBA=" << FBA_X;
		debug << " FIX=" << (u32) ALPHA_X.FIX;
		debug << " A=" << (u32) uA;
		debug << " B=" << (u32) uB;
		debug << " C=" << (u32) uC;
		debug << " D=" << (u32) uD;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}

	// calculate across
	if ( denominator )
	{
		// t0, t1 is in x.4 fixed point, denominator is in x.8 fixed point
		// dzdx is in x.23 fixed point (leaves 1 bit for possible sign)
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;
	}
	
	
	///////////////////////////////////////////
	// start at y0
	//Line = y0;
	
	
	//if ( denominator < 0 )
	//{
		// x1 is on the left and x0 is on the right //
		
		////////////////////////////////////
		// get slopes
		
		// start by assuming x1 is on the left and x0 is on the right //
		
		if ( y1 - y0 )
		{
			// triangle is pointed on top //
			
			/////////////////////////////////////////////
			// init x on the left and right
			
			// should be in point 16 fixed point format, where x0 is in point 4 fixed point
			//x [ 0 ] = ( ((s64)x0) << 32 );
			x [ 0 ] = ( ((s64)x0) << 12 );
			x [ 1 ] = x [ 0 ];
			
			// z coords (shift 31 to leave extra bit for a sign while interpolating?)
			z [ 0 ] = z0 << 23;
			z [ 1 ] = z [ 0 ];
			
			// should put dxdy in point 12 fixed point format
			dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			// z coords
			dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		}
		else
		{
			// Triangle is flat on top //
			
			// change x_left and x_right where y1 is on left
			// should be in point 16 fixed point format, where x0 is in point 4 fixed point
			//x [ X1Index ] = ( ((s64)x1) << 32 );
			//x [ X0Index ] = ( ((s64)x0) << 32 );
			x [ X1Index ] = ( ((s64)x1) << 12 );
			x [ X0Index ] = ( ((s64)x0) << 12 );
			
			z [ X1Index ] = ( ((s64)z1) << 23 );
			z [ X0Index ] = ( ((s64)z0) << 23 );
			
			// this means that x0 and x1 are on the same line
			// so if the height of the entire polygon is zero, then we are done
			if ( y2 - y1 )
			{
				// should put dxdy in point 12 fixed point format, where x and y are in point 4 format
				dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
				dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
				
				dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
				dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			}
		}
	//}

	/////////////////////////////////////////////////
	// swap left and right if they are reversed
	/*
	if ( denominator > 0 )
	{
		// x1,y1 is on the right //
		
		Swap ( x_left, x_right );
		Swap ( dx_left, dx_right );
	}
	*/

	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	////////////////
	// *** TODO *** at this point area of full triangle can be calculated and the rest of the drawing can be put on another thread *** //
	
	
	
	//StartY = y0;
	//EndY = y1;

	// left point is included if points are equal
	//StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	//EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	// todo: in this case, if EndY < StartY you should actually not draw the triangle (since coords include fractional parts on ps2)
	//if ( EndY < StartY ) EndY = StartY;
	
	
	// now y values are all integer
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	
	// go to next whole line
	Temp = ( StartY << 4 ) - y0;
	
	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;

	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	//z [ 1 ] += ( dzdy [ 1 ] >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;
		
		
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		// *** TODO *** need to update z going across after modifying starting x coords
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iZ = z [ 0 ];
			
			//if ( StartX < Window_XLeft )
			//{
			//	//Temp = DrawArea_TopLeftX - StartX;
			//	StartX = Window_XLeft;
			//}
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			/*
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			
			

			// draw horizontal line
			// *** TODO *** ptr16 could be anything here, so should not compare it against PtrEnd
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
			
				PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );

				iZ += dzdx;
				
			} // end for ( x_across = StartX; x_across <= EndX && ptr16 < PtrEnd; x_across += c_iVectorSize )
			
		} // end if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
		//z [ 1 ] += dzdy [ 1 ];
	} // end for ( Line = StartY; Line <= EndY; Line++ )

	
	} // end if ( EndY >= StartY )
	
	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	//////////////////////////////////////////////////////
	// check if y1 is on the left or on the right
	/*
	if ( denominator < 0 )
	{
		// y1 is on the left //
		
		x_left = ( ((s64)x1) << 32 );
		
		if ( y2 - y1 )
		{
			dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
		}
	}
	else
	{
		// y1 is on the right //
		
		x_right = ( ((s64)x1) << 32 );
		
		if ( y2 - y1 )
		{
			dx_right = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
		}
	}
	*/

	
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	
	// check if triangle is flat on the bottom //
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
	}
	
	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		//z [ 1 ] += ( dzdy [ 1 ] >> 4 ) * Temp;
	}
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;
		
		
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		// *** TODO *** need to update z going across after modifying starting x coords
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iZ = z [ 0 ];
			
			//if ( StartX < Window_XLeft )
			//{
			//	//Temp = DrawArea_TopLeftX - StartX;
			//	StartX = Window_XLeft;
			//}
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			/*
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			
			

			// draw horizontal line
			// *** TODO *** ptr16 could be anything here, so should not compare it against PtrEnd
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
			
				PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );

				iZ += dzdx;
				
			} // end for ( x_across = StartX; x_across <= EndX && ptr16 < PtrEnd; x_across += c_iVectorSize )
			
		} // end if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
		//z [ 1 ] += dzdy [ 1 ];
	} // end for ( Line = StartY; Line <= EndY; Line++ )
	
	} // end if ( EndY >= StartY )

}


#ifdef _ENABLE_SSE41_TRIANGLE_MONO

void GPU::DrawTriangle_Mono32_DSx4 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 4;

	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;
	
	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	u32 X1Index = 0, X0Index = 1;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;
	
	//u32 DestPixel;


	__m128i vEnable;
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 ( 3, 2, 1, 0 );
	
	__m128i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m128i vXInc;
	__m128i vYInc;
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vYInc = _mm_set1_epi32 ( 1 );
	
	__m128i vZInc;
	__m128i vZIdentity0, vZIdentity1;
	__m128i vZacross0, vZacross1;
	
	
	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	// get the color for the triangle
	// ***TODO*** rgbaq_Current is just the value of the RGBAQ register, so this should be fixed so rgbaq_Current variable is not needed
	//bgr = rgbaq [ Coord0 ].Value & 0xffffffffULL;
	bgr = rgbaq_Current.Value & 0xffffffffULL;
	
	
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	//if ( y1 < y0 )
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	//if ( y2 < y0 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	///////////////////////////////////////
	// put middle coordinates in x1,y1
	//if ( y2 < y1 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX = ( LeftMostX + 0xf ) >> 4;
	RightMostX = ( RightMostX - 1 ) >> 4;
	TopMostY = ( y0 + 0xf ) >> 4;
	BottomMostY = ( y2 - 1 ) >> 4;

	// check if triangle is within draw area
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleMono" << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	if ( ( ( RightMostX - LeftMostX ) > c_MaxPolygonWidth ) || ( ( BottomMostY - TopMostY ) > c_MaxPolygonHeight ) )
	{
		// skip drawing polygon
		return;
	}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}

	// calculate across
	if ( denominator )
	{
		// t0, t1 is in x.4 fixed point, denominator is in x.8 fixed point
		// dzdx is in x.23 fixed point (leaves 1 bit for possible sign)
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;
	}
	
	
	
	
	//if ( denominator < 0 )
	//{
		// x1 is on the left and x0 is on the right //
		
		////////////////////////////////////
		// get slopes
		
		// start by assuming x1 is on the left and x0 is on the right //
		
		if ( y1 - y0 )
		{
			// triangle is pointed on top //
			
			/////////////////////////////////////////////
			// init x on the left and right
			
			// should be in point 16 fixed point format, where x0 is in point 4 fixed point
			//x [ 0 ] = ( ((s64)x0) << 32 );
			x [ 0 ] = ( ((s64)x0) << 12 );
			x [ 1 ] = x [ 0 ];
			
			// z coords (shift 31 to leave extra bit for a sign while interpolating?)
			z [ 0 ] = z0 << 23;
			z [ 1 ] = z [ 0 ];
			
			// should put dxdy in point 12 fixed point format
			dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			// z coords
			dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		}
		else
		{
			// Triangle is flat on top //
			
			// change x_left and x_right where y1 is on left
			// should be in point 16 fixed point format, where x0 is in point 4 fixed point
			x [ X1Index ] = ( ((s64)x1) << 12 );
			x [ X0Index ] = ( ((s64)x0) << 12 );
			
			z [ X1Index ] = ( ((s64)z1) << 23 );
			z [ X0Index ] = ( ((s64)z0) << 23 );
			
			// this means that x0 and x1 are on the same line
			// so if the height of the entire polygon is zero, then we are done
			if ( y2 - y1 )
			{
				// should put dxdy in point 12 fixed point format, where x and y are in point 4 format
				dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
				dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
				
				dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
				dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			}
		}
	//}

	
	////////////////
	// *** TODO *** at this point area of full triangle can be calculated and the rest of the drawing can be put on another thread *** //
	
	
	
	
	
	// now y values are all integer
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	
	// go to next whole line
	Temp = ( StartY << 4 ) - y0;
	
	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;

	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	vBgr = _mm_set1_epi32 ( bgr );
	
	vAlphaSelect [ 0 ] = _mm_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm_set1_epi32 ( AlphaSelect [ 3 ] );

	vZIdentity0 = _mm_set_epi64x ( dzdx * 2, 0 );
	vZIdentity1 = _mm_set_epi64x ( dzdx * 3, dzdx * 1 );
	vZInc = _mm_set1_epi64x ( dzdx * c_iVectorSize );
	
	if ( EndY >= StartY )
	{

	vYacross = _mm_set1_epi32 ( StartY );
	
	//vZInc = _mm_set1_epi32 ( dzdx * c_iVectorSize );
	//vZIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dzdx ), vIdentity );
	

	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;

		// *** TODO *** need to update z going across after modifying starting x coords
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );

			// draw horizontal line
			// *** TODO *** ptr16 could be anything here, so should not compare it against PtrEnd
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				//PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm_srli_epi32 ( vZacross, 23 ), vBgr, _mm_cmplt_epi32 ( vXacross, vXmax ) );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm_set_epi32 ( ( iZ + ( ( dzdx << 1 ) + dzdx ) ) >> 23, ( iZ + ( dzdx << 1 ) ) >> 23, ( iZ + dzdx ) >> 23, iZ >> 23 ), vBgr, _mm_cmplt_epi32 ( vXacross, vXmax ) );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );
				
			} // end for ( x_across = StartX; x_across <= EndX && ptr16 < PtrEnd; x_across += c_iVectorSize )
			
		} // end if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm_add_epi32 ( vYacross, vYInc );
		
	} // end for ( Line = StartY; Line <= EndY; Line++ )

	
	} // end if ( EndY >= StartY )
	
	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	

	
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	
	// check if triangle is flat on the bottom //
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
	}
	

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		
	}
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{

	vYacross = _mm_set1_epi32 ( StartY );
	
	//vZInc = _mm_set1_epi32 ( dzdx * c_iVectorSize );
	//vZIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dzdx ), vIdentity );
	

	//////////////////////////////////////////////
	// draw down to y2
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		// *** TODO *** need to update z going across after modifying starting x coords
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			

			// draw horizontal line
			// *** TODO *** ptr16 could be anything here, so should not compare it against PtrEnd
			// x_left and x_right need to be rounded off
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				//PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm_srli_epi32 ( vZacross, 23 ), vBgr, _mm_cmplt_epi32 ( vXacross, vXmax ) );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm_set_epi32 ( ( iZ + ( ( dzdx << 1 ) + dzdx ) ) >> 23, ( iZ + ( dzdx << 1 ) ) >> 23, ( iZ + dzdx ) >> 23, iZ >> 23 ), vBgr, _mm_cmplt_epi32 ( vXacross, vXmax ) );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );
				
			} // end for ( x_across = StartX; x_across <= EndX && ptr16 < PtrEnd; x_across += c_iVectorSize )
			
		} // end if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm_add_epi32 ( vYacross, vYInc );
		
	} // end for ( Line = StartY; Line <= EndY; Line++ )
	
	} // end if ( EndY >= StartY )

}

#endif

#ifdef _ENABLE_AVX2_TRIANGLE_MONO

void GPU::DrawTriangle_Mono32_DSx8 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 8;

	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;
	
	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	u32 X1Index = 0, X0Index = 1;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;
	
	//u32 DestPixel;


	__m256i vEnable;
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 ( 7, 6, 5, 4, 3, 2, 1, 0 );
	
	__m256i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m256i vXInc;
	__m256i vYInc;
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vYInc = _mm256_set1_epi32 ( 1 );
	
	__m256i vZInc;
	__m256i vZIdentity0, vZIdentity1;
	__m256i vZacross0, vZacross1;
	
	
	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	// get the color for the triangle
	// ***TODO*** rgbaq_Current is just the value of the RGBAQ register, so this should be fixed so rgbaq_Current variable is not needed
	//bgr = rgbaq [ Coord0 ].Value & 0xffffffffULL;
	bgr = rgbaq_Current.Value & 0xffffffffULL;
	
	
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	//if ( y1 < y0 )
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	//if ( y2 < y0 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	///////////////////////////////////////
	// put middle coordinates in x1,y1
	//if ( y2 < y1 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX = ( LeftMostX + 0xf ) >> 4;
	RightMostX = ( RightMostX - 1 ) >> 4;
	TopMostY = ( y0 + 0xf ) >> 4;
	BottomMostY = ( y2 - 1 ) >> 4;

	// check if triangle is within draw area
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleMono" << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	if ( ( ( RightMostX - LeftMostX ) > c_MaxPolygonWidth ) || ( ( BottomMostY - TopMostY ) > c_MaxPolygonHeight ) )
	{
		// skip drawing polygon
		return;
	}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}

	// calculate across
	if ( denominator )
	{
		// t0, t1 is in x.4 fixed point, denominator is in x.8 fixed point
		// dzdx is in x.23 fixed point (leaves 1 bit for possible sign)
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;
	}
	
	
	
	
	//if ( denominator < 0 )
	//{
		// x1 is on the left and x0 is on the right //
		
		////////////////////////////////////
		// get slopes
		
		// start by assuming x1 is on the left and x0 is on the right //
		
		if ( y1 - y0 )
		{
			// triangle is pointed on top //
			
			/////////////////////////////////////////////
			// init x on the left and right
			
			// should be in point 16 fixed point format, where x0 is in point 4 fixed point
			//x [ 0 ] = ( ((s64)x0) << 32 );
			x [ 0 ] = ( ((s64)x0) << 12 );
			x [ 1 ] = x [ 0 ];
			
			// z coords (shift 31 to leave extra bit for a sign while interpolating?)
			z [ 0 ] = z0 << 23;
			z [ 1 ] = z [ 0 ];
			
			// should put dxdy in point 12 fixed point format
			dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			// z coords
			dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		}
		else
		{
			// Triangle is flat on top //
			
			// change x_left and x_right where y1 is on left
			// should be in point 16 fixed point format, where x0 is in point 4 fixed point
			x [ X1Index ] = ( ((s64)x1) << 12 );
			x [ X0Index ] = ( ((s64)x0) << 12 );
			
			z [ X1Index ] = ( ((s64)z1) << 23 );
			z [ X0Index ] = ( ((s64)z0) << 23 );
			
			// this means that x0 and x1 are on the same line
			// so if the height of the entire polygon is zero, then we are done
			if ( y2 - y1 )
			{
				// should put dxdy in point 12 fixed point format, where x and y are in point 4 format
				dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
				dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
				
				dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
				dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			}
		}
	//}

	
	////////////////
	// *** TODO *** at this point area of full triangle can be calculated and the rest of the drawing can be put on another thread *** //
	
	
	
	
	
	// now y values are all integer
	StartY = ( y0 + 0xf ) >> 4;
	EndY = ( y1 - 1 ) >> 4;
	
	// go to next whole line
	Temp = ( StartY << 4 ) - y0;
	
	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;

	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	vBgr = _mm256_set1_epi32 ( bgr );
	
	vAlphaSelect [ 0 ] = _mm256_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm256_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm256_set1_epi32 ( AlphaSelect [ 3 ] );

	vZIdentity0 = _mm256_set_epi64x ( dzdx * 6, dzdx * 4, dzdx * 2, 0 );
	vZIdentity1 = _mm256_set_epi64x ( dzdx * 7, dzdx * 5, dzdx * 3, dzdx * 1 );
	vZInc = _mm256_set1_epi64x ( dzdx * c_iVectorSize );
	
	if ( EndY >= StartY )
	{

	vYacross = _mm256_set1_epi32 ( StartY );
	
	//vZInc = _mm256_set1_epi32 ( dzdx * c_iVectorSize );
	//vZIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dzdx ), vIdentity );
	

	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;

		// *** TODO *** need to update z going across after modifying starting x coords
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );

			// draw horizontal line
			// *** TODO *** ptr16 could be anything here, so should not compare it against PtrEnd
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				//PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm256_srli_epi32 ( vZacross, 23 ), vBgr, _mm256_cmplt_epi32 ( vXacross, vXmax ) );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm256_set_epi32 ( ( iZ + ( ( dzdx << 1 ) + dzdx ) ) >> 23, ( iZ + ( dzdx << 1 ) ) >> 23, ( iZ + dzdx ) >> 23, iZ >> 23 ), vBgr, _mm256_cmplt_epi32 ( vXacross, vXmax ) );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );
				
			} // end for ( x_across = StartX; x_across <= EndX && ptr16 < PtrEnd; x_across += c_iVectorSize )
			
		} // end if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm256_add_epi32 ( vYacross, vYInc );
		
	} // end for ( Line = StartY; Line <= EndY; Line++ )

	
	} // end if ( EndY >= StartY )
	
	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	

	
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	
	// check if triangle is flat on the bottom //
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
	}
	

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		
	}
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{

	vYacross = _mm256_set1_epi32 ( StartY );
	
	//vZInc = _mm256_set1_epi32 ( dzdx * c_iVectorSize );
	//vZIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dzdx ), vIdentity );
	

	//////////////////////////////////////////////
	// draw down to y2
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		// *** TODO *** need to update z going across after modifying starting x coords
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			

			// draw horizontal line
			// *** TODO *** ptr16 could be anything here, so should not compare it against PtrEnd
			// x_left and x_right need to be rounded off
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
			
				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				//PlotPixel_Mono ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm256_srli_epi32 ( vZacross, 23 ), vBgr, _mm256_cmplt_epi32 ( vXacross, vXmax ) );
				//PlotPixel_Monox4 ( vXacross, vYacross, _mm256_set_epi32 ( ( iZ + ( ( dzdx << 1 ) + dzdx ) ) >> 23, ( iZ + ( dzdx << 1 ) ) >> 23, ( iZ + dzdx ) >> 23, iZ >> 23 ), vBgr, _mm256_cmplt_epi32 ( vXacross, vXmax ) );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );
				
			} // end for ( x_across = StartX; x_across <= EndX && ptr16 < PtrEnd; x_across += c_iVectorSize )
			
		} // end if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm256_add_epi32 ( vYacross, vYInc );
		
	} // end for ( Line = StartY; Line <= EndY; Line++ )
	
	} // end if ( EndY >= StartY )

}

#endif



void GPU::DrawTriangle_Gradient32_DS ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 1;

	//u32 Pixel, TexelIndex,
	
	//u32 Y1_OnLeft;
	
	//u32 color_add;
	
	//u16 *ptr;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	//s64 Error_Left;
	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	s64 r [ 2 ], g [ 2 ], b [ 2 ];
	s64 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	s64 iR, iG, iB;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	s64 iA;
	s64 a [ 2 ];
	s64 dady [ 2 ];
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;
	
	
	/*
	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	u32 *VRAM32;
	u16 *VRAM16;
	
	u32 *buf32;
	u16 *buf16;
	*/
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	s32 drdx, dgdx, dbdx;
	s32 dadx;
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel;
	
	u32 PixelMask = 0, SetPixelMask = 0;
	
	// alpha color component, like Red, Green, etc above
	s64 Alp;

	/*
	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];
	
	u32 PixelAlpha, PixelAlpha16;


	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	
	// z-buffer
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;


	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;
	
	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	
	//if ( GPU_CTRL_Read.ME ) PixelMask = 0x10000;
	//if ( GPU_CTRL_Read.MD ) SetPixelMask = 0x10000;

	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;
	*/

	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	//if ( GPU_CTRL_Read.ME ) PixelMask = 0x10000;
	//if ( GPU_CTRL_Read.MD ) SetPixelMask = 0x10000;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	
	///////////////////////////////////////////////////
	// Initialize dithering
	/*
	DitherArray = c_iDitherZero;
	
	if ( GPU_CTRL_Read.DTD )
	{
		//DitherArray = c_iDitherValues;
		DitherArray = c_iDitherValues24;
	}
	*/
	
	
	/*
	///////////////////////////////////
	// put top coordinates in x0,y0
	VRAM32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	VRAM16 = (u16*) VRAM32;
	
	buf32 = VRAM32;
	buf16 = VRAM16;
	*/
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	//if ( y1 < y0 )
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	//if ( y2 < y0 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	///////////////////////////////////////
	// put middle coordinates in x1,y1
	//if ( y2 < y1 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// invert z for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
	z2 ^= 0xffffffffULL;
#endif
	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	// coords are in 12.4 fixed point
	//x0 >>= 4;
	//y0 >>= 4;
	//x1 >>= 4;
	//y1 >>= 4;
	//x2 >>= 4;
	//y2 >>= 4;
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleGradient";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " rgba0=" << ( rgbaq [ Coord0 ].Value & 0xffffffff ) << " rgba1=" << ( rgbaq [ Coord1 ].Value & 0xffffffff ) << " rgba2=" << ( rgbaq [ Coord2 ].Value & 0xffffffff );
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		if ( Alpha )
		{
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		}
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	// x.4 * x.4 = x.8, t is in x.4 since y is in x.4, and x is in x.4, so denominator is in x.8
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//drdx = ( ( (s64) ( ( ( r0 - r2 ) * t0 ) - ( ( r1 - r2 ) * t1 ) ) ) << 24 ) / denominator;
		//dgdx = ( ( (s64) ( ( ( g0 - g2 ) * t0 ) - ( ( g1 - g2 ) * t1 ) ) ) << 24 ) / denominator;
		//dbdx = ( ( (s64) ( ( ( b0 - b2 ) * t0 ) - ( ( b1 - b2 ) * t1 ) ) ) << 24 ) / denominator;
		
		// result here should be in x.24 fixed point for now
		drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << 28 ) / denominator;
		dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << 28 ) / denominator;
		dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << 28 ) / denominator;
		dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << 28 ) / denominator;
		
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
//debug << hex << "\r\ndrdx=" << drdx << " dgdx=" << dgdx << " dbdx=" << dbdx << " dadx=" << dadx << " dzdx=" << dzdx;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	r [ X0Index ] = ( rgbaq [ Coord0 ].R << 24 );
	g [ X0Index ] = ( rgbaq [ Coord0 ].G << 24 );
	b [ X0Index ] = ( rgbaq [ Coord0 ].B << 24 );
	
	a [ X0Index ] = ( rgbaq [ Coord0 ].A << 24 );
	
	z [ X0Index ] = ( z0 << 23 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		r [ X1Index ] = r [ X0Index ];
		g [ X1Index ] = g [ X0Index ];
		b [ X1Index ] = b [ X0Index ];
		
		a [ X1Index ] = a [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		
		// the x and y values are in 12.4 fixed point, so need to make signed 64-bit before shifting up
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].R - rgbaq [ Coord0 ].R )) << 28 ) / ((s64)( y1 - y0 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].G - rgbaq [ Coord0 ].G )) << 28 ) / ((s64)( y1 - y0 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].B - rgbaq [ Coord0 ].B )) << 28 ) / ((s64)( y1 - y0 ));
		
		dady [ X1Index ] = (((s64)( rgbaq [ Coord1 ].A - rgbaq [ Coord0 ].A )) << 28 ) / ((s64)( y1 - y0 ));
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		
		drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 28 ) / ((s64)( y2 - y0 ));
		dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 28 ) / ((s64)( y2 - y0 ));
		dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 28 ) / ((s64)( y2 - y0 ));
		
		dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 28 ) / ((s64)( y2 - y0 ));
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
		g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
		b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
		
		a [ X1Index ] = ( rgbaq [ Coord1 ].A << 24 );
		z [ X1Index ] = ( z1 << 23 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			// only need to set dr,dg,db for the x0/x2 side here
			drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 28 ) / ((s64)( y2 - y0 ));
			dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 28 ) / ((s64)( y2 - y0 ));
			dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 28 ) / ((s64)( y2 - y0 ));
			
			dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 28 ) / ((s64)( y2 - y0 ));
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		}
	}
	
//debug << hex << "\r\ndrdy[0]=" << drdy[0] << " dgdy[0]=" << dgdy[0] << " dbdy[0]=" << dbdy[0] << " dady[0]=" << dady[0] << " dzdy[0]=" << dzdy[0];
//debug << hex << "\r\ndrdy[1]=" << drdy[1] << " dgdy[1]=" << dgdy[1] << " dbdy[1]=" << dbdy[1] << " dady[1]=" << dady[1] << " dzdy[1]=" << dzdy[1];
	
	
	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	
	//StartY = y0;
	//EndY = y1;

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
	g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
	b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
	
	a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	
	//r [ 1 ] += ( drdy [ 1 ] >> 4 ) * Temp;
	//g [ 1 ] += ( dgdy [ 1 ] >> 4 ) * Temp;
	//b [ 1 ] += ( dbdy [ 1 ] >> 4 ) * Temp;
	
	//a [ 1 ] += ( dady [ 1 ] >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;
		
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		//if ( EndX < StartX ) EndX = StartX;
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			
			iA = a [ 0 ];
			iZ = z [ 0 ];
			
//debug << "\r\niR=" << iR << " iG=" << iG << " iB=" << iB << " iA=" << iA << " iZ=" << iZ;
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iR += ( drdx >> 16 ) * Temp;
			iG += ( dgdx >> 16 ) * Temp;
			iB += ( dbdx >> 16 ) * Temp;
			
			iA += ( dadx >> 16 ) * Temp;
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/*
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			//DitherLine = & ( DitherArray [ ( Line & 0x3 ) << 2 ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
					
					
					// ***TODO*** this is a PROBLEM, since iR, iG, iB, iA are signed 32-bit values, so below can easily get wrong colors!
					Red = iR >> 24;
					Green = iG >> 24;
					Blue = iB >> 24;
					
					// get alpha component
					Alp = iA >> 24;
					
					// clamp
					// *** NOTE *** need to clamp here since we are using s32 instead of s64 for iR,iG,iB,iA
					Red &= 0xff;
					Green &= 0xff;
					Blue &= 0xff;
					Alp &= 0xff;
					
					//bgr = ( Blue << 10 ) | ( Green << 5 ) | Red;
					bgr = ( Blue << 16 ) | ( Green << 8 ) | Red;
					
					// add in the alpha component
					bgr |= ( Alp << 24 );
					
//debug << dec << " x=" << x_across << " y=" << Line << " bgr=" << hex << bgr;
					
					PlotPixel_Gradient ( x_across, Line, iZ >> 23, bgr );

						
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
			
				iA += dadx;
				iZ += dzdx;
			}
			
		}
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		//r [ 1 ] += drdy [ 1 ];
		//g [ 1 ] += dgdy [ 1 ];
		//b [ 1 ] += dbdy [ 1 ];
		
		a [ 0 ] += dady [ 0 ];
		//a [ 1 ] += dady [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
	}
	
	} // end if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
	g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
	b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );

	a [ X1Index ] = ( rgbaq [ Coord1 ].A << 24 );
	z [ X1Index ] = ( xyz [ Coord1 ].Z << 23 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		//dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 24 ) / ((s64)( y2 - y1 ));
		//dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 24 ) / ((s64)( y2 - y1 ));
		//dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 24 ) / ((s64)( y2 - y1 ));
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 28 ) / ((s64)( y2 - y1 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 28 ) / ((s64)( y2 - y1 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 28 ) / ((s64)( y2 - y1 ));
		
		dady [ X1Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord1 ].A )) << 28 ) / ((s64)( y2 - y1 ));
		dzdy [ X1Index ] = (((s64)( xyz [ Coord2 ].Z - xyz [ Coord1 ].Z )) << 27 ) / ((s64)( y2 - y1 ));
	}
	
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( (s64) ( y1 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y2 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	r [ X1Index ] += ( drdy [ X1Index ] >> 4 ) * Temp;
	g [ X1Index ] += ( dgdy [ X1Index ] >> 4 ) * Temp;
	b [ X1Index ] += ( dbdy [ X1Index ] >> 4 ) * Temp;
	
	a [ X1Index ] += ( dady [ X1Index ] >> 4 ) * Temp;
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
		g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
		b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;

		a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		
		//r [ 1 ] += ( drdy [ 1 ] >> 4 ) * Temp;
		//g [ 1 ] += ( dgdy [ 1 ] >> 4 ) * Temp;
		//b [ 1 ] += ( dbdy [ 1 ] >> 4 ) * Temp;
		
		//a [ 1 ] += ( dady [ 1 ] >> 4 ) * Temp;
	}
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;
		
		// left point is included if points are equal
		StartX = ( (s64) ( x [ 0 ] + 0xffffLL ) ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			
			iA = a [ 0 ];
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iR += ( drdx >> 16 ) * Temp;
			iG += ( dgdx >> 16 ) * Temp;
			iB += ( dbdx >> 16 ) * Temp;
			
			iA += ( dadx >> 16 ) * Temp;
			iZ += ( dzdx >> 16 ) * Temp;
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			/*
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			//DitherLine = & ( DitherArray [ ( Line & 0x3 ) << 2 ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
					
					Red = iR >> 24;
					Green = iG >> 24;
					Blue = iB >> 24;
					
					// get alpha component
					Alp = iA >> 24;
					
					// clamp
					// *** NOTE *** need to clamp here since we are using s32 instead of s64 for iR,iG,iB,iA
					Red &= 0xff;
					Green &= 0xff;
					Blue &= 0xff;
					Alp &= 0xff;
					
					//bgr = ( Blue << 10 ) | ( Green << 5 ) | Red;
					bgr = ( Blue << 16 ) | ( Green << 8 ) | Red;
					
					// add in alpha component
					bgr |= ( Alp << 24 );
					
					
					PlotPixel_Gradient ( x_across, Line, iZ >> 23, bgr );
					
					
					
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				
				iA += dadx;
				iZ += dzdx;
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		//r [ 1 ] += drdy [ 1 ];
		//g [ 1 ] += dgdy [ 1 ];
		//b [ 1 ] += dbdy [ 1 ];
		
		a [ 0 ] += dady [ 0 ];
		//a [ 1 ] += dady [ 1 ];
		
		z [ 0 ] += dzdy [ 0 ];
	}
	
	} // end if ( EndY >= StartY )
		
}

#ifdef _ENABLE_SSE41_TRIANGLE_GRADIENT

void GPU::DrawTriangle_Gradient32_DSx4 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 4;

	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	s64 r [ 2 ], g [ 2 ], b [ 2 ];
	s64 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	s64 iR, iG, iB;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	s64 iA;
	s64 a [ 2 ];
	s64 dady [ 2 ];
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;
	
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	s32 drdx, dgdx, dbdx;
	s32 dadx;
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel;
	
	u32 PixelMask = 0, SetPixelMask = 0;
	
	// alpha color component, like Red, Green, etc above
	s64 Alp;

	__m128i vEnable;
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 ( 3, 2, 1, 0 );
	
	__m128i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m128i vXInc;
	__m128i vYInc;
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vYInc = _mm_set1_epi32 ( 1 );
	
	__m128i vZInc;
	__m128i vZIdentity0, vZIdentity1;
	__m128i vZacross0, vZacross1;
	
	__m128i vRInc, vGInc, vBInc, vAInc;
	__m128i vRIdentity, vGIdentity, vBIdentity, vAIdentity;
	__m128i vRacross, vGacross, vBacross, vAacross;
	__m128i vRed, vGreen, vBlue, vAlpha;


	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	
	///////////////////////////////////////////////////
	// Initialize dithering

	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	//if ( y1 < y0 )
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	//if ( y2 < y0 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	///////////////////////////////////////
	// put middle coordinates in x1,y1
	//if ( y2 < y1 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX = ( LeftMostX + 0xf ) >> 4;
	RightMostX = ( RightMostX - 1 ) >> 4;
	TopMostY = ( y0 + 0xf ) >> 4;
	BottomMostY = ( y2 - 1 ) >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleGradient";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " rgba0=" << ( rgbaq [ Coord0 ].Value & 0xffffffff ) << " rgba1=" << ( rgbaq [ Coord1 ].Value & 0xffffffff ) << " rgba2=" << ( rgbaq [ Coord2 ].Value & 0xffffffff );
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		if ( Alpha )
		{
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		}
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	if ( ( ( RightMostX - LeftMostX ) > c_MaxPolygonWidth ) || ( ( BottomMostY - TopMostY ) > c_MaxPolygonHeight ) )
	{
		// skip drawing polygon
		return;
	}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	// x.4 * x.4 = x.8, t is in x.4 since y is in x.4, and x is in x.4, so denominator is in x.8
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		
		// result here should be in x.24 fixed point for now
		drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;
	}
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	r [ X0Index ] = ( rgbaq [ Coord0 ].R << 16 );
	g [ X0Index ] = ( rgbaq [ Coord0 ].G << 16 );
	b [ X0Index ] = ( rgbaq [ Coord0 ].B << 16 );
	
	a [ X0Index ] = ( rgbaq [ Coord0 ].A << 16 );
	
	z [ X0Index ] = ( z0 << 23 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		x [ X1Index ] = x [ X0Index ];
		
		r [ X1Index ] = r [ X0Index ];
		g [ X1Index ] = g [ X0Index ];
		b [ X1Index ] = b [ X0Index ];
		
		a [ X1Index ] = a [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		
		// the x and y values are in 12.4 fixed point, so need to make signed 64-bit before shifting up
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].R - rgbaq [ Coord0 ].R )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].G - rgbaq [ Coord0 ].G )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].B - rgbaq [ Coord0 ].B )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		
		dady [ X1Index ] = (((s64)( rgbaq [ Coord1 ].A - rgbaq [ Coord0 ].A )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		
		drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		
		dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
		g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
		b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );
		
		a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
		z [ X1Index ] = ( z1 << 23 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			// only need to set dr,dg,db for the x0/x2 side here
			drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			
			dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		}
	}
	
	
	

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
	g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
	b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
	
	a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}



	//vAlphaSelect [ 0 ] = _mm_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm_set1_epi32 ( AlphaSelect [ 3 ] );
	
	vZIdentity0 = _mm_set_epi64x ( dzdx * 2, 0 );
	vZIdentity1 = _mm_set_epi64x ( dzdx * 3, dzdx * 1 );
	vZInc = _mm_set1_epi64x ( dzdx * c_iVectorSize );
	
	vRIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( drdx ), vIdentity );
	vGIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dgdx ), vIdentity );
	vBIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dbdx ), vIdentity );
	vAIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dadx ), vIdentity );
	
	vRInc = _mm_set1_epi32 ( drdx * c_iVectorSize );
	vGInc = _mm_set1_epi32 ( dgdx * c_iVectorSize );
	vBInc = _mm_set1_epi32 ( dbdx * c_iVectorSize );
	vAInc = _mm_set1_epi32 ( dadx * c_iVectorSize );
	
	if ( EndY >= StartY )
	{
	
	vYacross = _mm_set1_epi32 ( StartY );
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			
			iA = a [ 0 ];
			iZ = z [ 0 ];
			
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			
			vRacross = _mm_add_epi32 ( _mm_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm_add_epi32 ( _mm_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm_add_epi32 ( _mm_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm_add_epi32 ( _mm_set1_epi32 ( iA ), vAIdentity );

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				/*
				// ***TODO*** this is a PROBLEM, since iR, iG, iB, iA are signed 32-bit values, so below can easily get wrong colors!
				Red = iR >> 24;
				Green = iG >> 24;
				Blue = iB >> 24;
				
				// get alpha component
				Alp = iA >> 24;
				
				// clamp
				// *** NOTE *** need to clamp here since we are using s32 instead of s64 for iR,iG,iB,iA
				Red &= 0xff;
				Green &= 0xff;
				Blue &= 0xff;
				Alp &= 0xff;
				*/
				
				vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vRacross, 8 ), 24 );
				vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vGacross, 8 ), 24 );
				vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBacross, 8 ), 24 );
				vAlpha = _mm_srli_epi32 ( _mm_slli_epi32 ( vAacross, 8 ), 24 );
				
				/*
				bgr = ( Blue << 16 ) | ( Green << 8 ) | Red;
				
				// add in the alpha component
				bgr |= ( Alp << 24 );
				*/
				
				vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 ) ), _mm_or_si128 ( _mm_slli_epi32 ( vBlue, 16 ), _mm_slli_epi32 ( vAlpha, 24 ) ) );
				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				vAlphaSelect [ 0 ] = vBgr;
				
				//PlotPixel_Gradient ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );

				/*
				iZ += dzdx;
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				*/
				
				vRacross = _mm_add_epi32 ( vRacross, vRInc );
				vGacross = _mm_add_epi32 ( vGacross, vGInc );
				vBacross = _mm_add_epi32 ( vBacross, vBInc );
				vAacross = _mm_add_epi32 ( vAacross, vAInc );
			}
			
		}
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm_add_epi32 ( vYacross, vYInc );
	}
	
	} // end if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
	g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
	b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );

	a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
	z [ X1Index ] = ( xyz [ Coord1 ].Z << 23 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 24 ) / ((s64)( y2 - y1 ));
		//dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 24 ) / ((s64)( y2 - y1 ));
		//dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 24 ) / ((s64)( y2 - y1 ));
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		
		dady [ X1Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord1 ].A )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		dzdy [ X1Index ] = (((s64)( xyz [ Coord2 ].Z - xyz [ Coord1 ].Z )) << 27 ) / ((s64)( y2 - y1 ));
	}
	
	

	// left point is included if points are equal
	StartY = ( (s64) ( y1 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y2 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	r [ X1Index ] += ( drdy [ X1Index ] >> 4 ) * Temp;
	g [ X1Index ] += ( dgdy [ X1Index ] >> 4 ) * Temp;
	b [ X1Index ] += ( dbdy [ X1Index ] >> 4 ) * Temp;
	
	a [ X1Index ] += ( dady [ X1Index ] >> 4 ) * Temp;
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
		g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
		b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;

		a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		
	}
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	vYacross = _mm_set1_epi32 ( StartY );
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( (s64) ( x [ 0 ] + 0xffffLL ) ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			
			iA = a [ 0 ];
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			iZ += ( dzdx >> 16 ) * Temp;
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;

			
			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			
			vRacross = _mm_add_epi32 ( _mm_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm_add_epi32 ( _mm_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm_add_epi32 ( _mm_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm_add_epi32 ( _mm_set1_epi32 ( iA ), vAIdentity );


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				/*
				// ***TODO*** this is a PROBLEM, since iR, iG, iB, iA are signed 32-bit values, so below can easily get wrong colors!
				Red = iR >> 24;
				Green = iG >> 24;
				Blue = iB >> 24;
				
				// get alpha component
				Alp = iA >> 24;
				
				// clamp
				// *** NOTE *** need to clamp here since we are using s32 instead of s64 for iR,iG,iB,iA
				Red &= 0xff;
				Green &= 0xff;
				Blue &= 0xff;
				Alp &= 0xff;
				*/
				
				vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vRacross, 8 ), 24 );
				vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vGacross, 8 ), 24 );
				vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBacross, 8 ), 24 );
				vAlpha = _mm_srli_epi32 ( _mm_slli_epi32 ( vAacross, 8 ), 24 );
				
				/*
				bgr = ( Blue << 16 ) | ( Green << 8 ) | Red;
				
				// add in the alpha component
				bgr |= ( Alp << 24 );
				*/
				
				vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 ) ), _mm_or_si128 ( _mm_slli_epi32 ( vBlue, 16 ), _mm_slli_epi32 ( vAlpha, 24 ) ) );
				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				vAlphaSelect [ 0 ] = vBgr;
				
				//PlotPixel_Gradient ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );

				/*
				iZ += dzdx;
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				*/
				
				vRacross = _mm_add_epi32 ( vRacross, vRInc );
				vGacross = _mm_add_epi32 ( vGacross, vGInc );
				vBacross = _mm_add_epi32 ( vBacross, vBInc );
				vAacross = _mm_add_epi32 ( vAacross, vAInc );
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm_add_epi32 ( vYacross, vYInc );
	}
	
	} // end if ( EndY >= StartY )
		
}

#endif

#ifdef _ENABLE_AVX2_TRIANGLE_GRADIENT

void GPU::DrawTriangle_Gradient32_DSx8 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 8;

	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	s64 r [ 2 ], g [ 2 ], b [ 2 ];
	s64 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	s64 iR, iG, iB;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	s64 iA;
	s64 a [ 2 ];
	s64 dady [ 2 ];
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;
	
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	s32 drdx, dgdx, dbdx;
	s32 dadx;
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel;
	
	u32 PixelMask = 0, SetPixelMask = 0;
	
	// alpha color component, like Red, Green, etc above
	s64 Alp;

	__m256i vEnable;
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 ( 7, 6, 5, 4, 3, 2, 1, 0 );
	
	__m256i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m256i vXInc;
	__m256i vYInc;
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vYInc = _mm256_set1_epi32 ( 1 );
	
	__m256i vZInc;
	__m256i vZIdentity0, vZIdentity1;
	__m256i vZacross0, vZacross1;
	
	__m256i vRInc, vGInc, vBInc, vAInc;
	__m256i vRIdentity, vGIdentity, vBIdentity, vAIdentity;
	__m256i vRacross, vGacross, vBacross, vAacross;
	__m256i vRed, vGreen, vBlue, vAlpha;


	// set fixed alpha values
	AlphaSelect [ 0 ] = rgbaq_Current.Value & 0xffffffffL;
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	
	///////////////////////////////////////////////////
	// Initialize dithering

	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	//if ( y1 < y0 )
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	//if ( y2 < y0 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	///////////////////////////////////////
	// put middle coordinates in x1,y1
	//if ( y2 < y1 )
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;
	
	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 );
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX = ( LeftMostX + 0xf ) >> 4;
	RightMostX = ( RightMostX - 1 ) >> 4;
	TopMostY = ( y0 + 0xf ) >> 4;
	BottomMostY = ( y2 - 1 ) >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleGradient";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " rgba0=" << ( rgbaq [ Coord0 ].Value & 0xffffffff ) << " rgba1=" << ( rgbaq [ Coord1 ].Value & 0xffffffff ) << " rgba2=" << ( rgbaq [ Coord2 ].Value & 0xffffffff );
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		if ( Alpha )
		{
		debug << " uA=" << hex << uA;
		debug << " uB=" << hex << uB;
		debug << " uC=" << hex << uC;
		debug << " uD=" << hex << uD;
		}
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	if ( ( ( RightMostX - LeftMostX ) > c_MaxPolygonWidth ) || ( ( BottomMostY - TopMostY ) > c_MaxPolygonHeight ) )
	{
		// skip drawing polygon
		return;
	}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	// x.4 * x.4 = x.8, t is in x.4 since y is in x.4, and x is in x.4, so denominator is in x.8
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		
		// result here should be in x.24 fixed point for now
		drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;
	}
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	r [ X0Index ] = ( rgbaq [ Coord0 ].R << 16 );
	g [ X0Index ] = ( rgbaq [ Coord0 ].G << 16 );
	b [ X0Index ] = ( rgbaq [ Coord0 ].B << 16 );
	
	a [ X0Index ] = ( rgbaq [ Coord0 ].A << 16 );
	
	z [ X0Index ] = ( z0 << 23 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		x [ X1Index ] = x [ X0Index ];
		
		r [ X1Index ] = r [ X0Index ];
		g [ X1Index ] = g [ X0Index ];
		b [ X1Index ] = b [ X0Index ];
		
		a [ X1Index ] = a [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		
		// the x and y values are in 12.4 fixed point, so need to make signed 64-bit before shifting up
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].R - rgbaq [ Coord0 ].R )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].G - rgbaq [ Coord0 ].G )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].B - rgbaq [ Coord0 ].B )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		
		dady [ X1Index ] = (((s64)( rgbaq [ Coord1 ].A - rgbaq [ Coord0 ].A )) << ( 16 + 4 ) ) / ((s64)( y1 - y0 ));
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		
		drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		
		dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
		g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
		b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );
		
		a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
		z [ X1Index ] = ( z1 << 23 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			// only need to set dr,dg,db for the x0/x2 side here
			drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			
			dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << ( 16 + 4 ) ) / ((s64)( y2 - y0 ));
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		}
	}
	
	
	

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
	g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
	b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
	
	a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}



	//vAlphaSelect [ 0 ] = _mm256_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm256_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm256_set1_epi32 ( AlphaSelect [ 3 ] );
	
	vZIdentity0 = _mm256_set_epi64x ( dzdx * 6, dzdx * 4, dzdx * 2, 0 );
	vZIdentity1 = _mm256_set_epi64x ( dzdx * 7, dzdx * 5, dzdx * 3, dzdx * 1 );
	vZInc = _mm256_set1_epi64x ( dzdx * c_iVectorSize );
	
	vRIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( drdx ), vIdentity );
	vGIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dgdx ), vIdentity );
	vBIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dbdx ), vIdentity );
	vAIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dadx ), vIdentity );
	
	vRInc = _mm256_set1_epi32 ( drdx * c_iVectorSize );
	vGInc = _mm256_set1_epi32 ( dgdx * c_iVectorSize );
	vBInc = _mm256_set1_epi32 ( dbdx * c_iVectorSize );
	vAInc = _mm256_set1_epi32 ( dadx * c_iVectorSize );
	
	if ( EndY >= StartY )
	{
	
	vYacross = _mm256_set1_epi32 ( StartY );
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			
			iA = a [ 0 ];
			iZ = z [ 0 ];
			
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			iZ += ( dzdx >> 16 ) * Temp;
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			
			vRacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iA ), vAIdentity );

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				/*
				// ***TODO*** this is a PROBLEM, since iR, iG, iB, iA are signed 32-bit values, so below can easily get wrong colors!
				Red = iR >> 24;
				Green = iG >> 24;
				Blue = iB >> 24;
				
				// get alpha component
				Alp = iA >> 24;
				
				// clamp
				// *** NOTE *** need to clamp here since we are using s32 instead of s64 for iR,iG,iB,iA
				Red &= 0xff;
				Green &= 0xff;
				Blue &= 0xff;
				Alp &= 0xff;
				*/
				
				vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vRacross, 8 ), 24 );
				vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vGacross, 8 ), 24 );
				vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBacross, 8 ), 24 );
				vAlpha = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vAacross, 8 ), 24 );
				
				/*
				bgr = ( Blue << 16 ) | ( Green << 8 ) | Red;
				
				// add in the alpha component
				bgr |= ( Alp << 24 );
				*/
				
				vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 ) ), _mm256_or_si256 ( _mm256_slli_epi32 ( vBlue, 16 ), _mm256_slli_epi32 ( vAlpha, 24 ) ) );
				
				vAlphaSelect [ 0 ] = vBgr;
				
				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				//PlotPixel_Gradient ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );

				/*
				iZ += dzdx;
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				*/
				
				vRacross = _mm256_add_epi32 ( vRacross, vRInc );
				vGacross = _mm256_add_epi32 ( vGacross, vGInc );
				vBacross = _mm256_add_epi32 ( vBacross, vBInc );
				vAacross = _mm256_add_epi32 ( vAacross, vAInc );
			}
			
		}
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm256_add_epi32 ( vYacross, vYInc );
	}
	
	} // end if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
	g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
	b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );

	a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
	z [ X1Index ] = ( xyz [ Coord1 ].Z << 23 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 24 ) / ((s64)( y2 - y1 ));
		//dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 24 ) / ((s64)( y2 - y1 ));
		//dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 24 ) / ((s64)( y2 - y1 ));
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		
		dady [ X1Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord1 ].A )) << ( 16 + 4 ) ) / ((s64)( y2 - y1 ));
		dzdy [ X1Index ] = (((s64)( xyz [ Coord2 ].Z - xyz [ Coord1 ].Z )) << 27 ) / ((s64)( y2 - y1 ));
	}
	
	

	// left point is included if points are equal
	StartY = ( (s64) ( y1 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y2 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	r [ X1Index ] += ( drdy [ X1Index ] >> 4 ) * Temp;
	g [ X1Index ] += ( dgdy [ X1Index ] >> 4 ) * Temp;
	b [ X1Index ] += ( dbdy [ X1Index ] >> 4 ) * Temp;
	
	a [ X1Index ] += ( dady [ X1Index ] >> 4 ) * Temp;
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
		g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
		b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;

		a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		
	}
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	vYacross = _mm256_set1_epi32 ( StartY );
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( (s64) ( x [ 0 ] + 0xffffLL ) ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			
			iA = a [ 0 ];
			iZ = z [ 0 ];
			
			// get distance from point to pixel
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			iZ += ( dzdx >> 16 ) * Temp;
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;

			
			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			
			vRacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iA ), vAIdentity );


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				/*
				// ***TODO*** this is a PROBLEM, since iR, iG, iB, iA are signed 32-bit values, so below can easily get wrong colors!
				Red = iR >> 24;
				Green = iG >> 24;
				Blue = iB >> 24;
				
				// get alpha component
				Alp = iA >> 24;
				
				// clamp
				// *** NOTE *** need to clamp here since we are using s32 instead of s64 for iR,iG,iB,iA
				Red &= 0xff;
				Green &= 0xff;
				Blue &= 0xff;
				Alp &= 0xff;
				*/
				
				vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vRacross, 8 ), 24 );
				vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vGacross, 8 ), 24 );
				vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBacross, 8 ), 24 );
				vAlpha = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vAacross, 8 ), 24 );
				
				/*
				bgr = ( Blue << 16 ) | ( Green << 8 ) | Red;
				
				// add in the alpha component
				bgr |= ( Alp << 24 );
				*/
				
				vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 ) ), _mm256_or_si256 ( _mm256_slli_epi32 ( vBlue, 16 ), _mm256_slli_epi32 ( vAlpha, 24 ) ) );
				
				vAlphaSelect [ 0 ] = vBgr;
				
				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				//PlotPixel_Gradient ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				//iZ += dzdx;
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );

				/*
				iZ += dzdx;
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				*/
				
				vRacross = _mm256_add_epi32 ( vRacross, vRInc );
				vGacross = _mm256_add_epi32 ( vGacross, vGInc );
				vBacross = _mm256_add_epi32 ( vBacross, vBInc );
				vAacross = _mm256_add_epi32 ( vAacross, vAInc );
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		
		vYacross = _mm256_add_epi32 ( vYacross, vYInc );
	}
	
	} // end if ( EndY >= StartY )
		
}

#endif


void GPU::DrawTriangle_Texture32_DS ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 1;

	//u32 Pixel, TexelIndex,
	
	//u32 Y1_OnLeft;
	
	u16 *CLUT_LUT;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;
	
	u32 *ptr_texture32;
	u16 *ptr_texture16;
	u8 *ptr_texture8;
	
	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	//s64 Error_Left;
	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	
	float fS0, fS1, fS2, fT0, fT1, fT2, fQ0, fQ1, fQ2;
	float fS [ 2 ], fT [ 2 ], fQ [ 2 ], dSdy [ 2 ], dTdy [ 2 ], dQdy [ 2 ];
	float iS, iT, iQ, dSdx, dTdx, dQdx;
	
	s64 iU, iV;
	
	//s32 iR, iG, iB;
	
	s64 u [ 2 ], v [ 2 ];
	s64 dudy [ 2 ], dvdy [ 2 ];
	
	//u32 r [ 2 ], g [ 2 ], b [ 2 ];
	//s32 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	
	s64 dudx, dvdx;
	//s32 drdx, dgdx, dbdx;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;

	// fog coords ?
	s64 f0, f1, f2;
	s64 fogc [ 2 ], dfdy [ 2 ];
	s64 dfdx;
	s64 iF;
	
	u32 c1, c2, c3, ca;
	
	/*
	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	u32 *VRAM32;
	u16 *VRAM16;
	
	u32 *buf32;
	u16 *buf16;
	*/
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel, PixelMask = 0, SetPixelMask = 0;


	s32 TexCoordX, TexCoordY;
	u32 Shift1 = 0, Shift2 = 0, And1 = 0, And2 = 0;
	u32 Shift0 = 0;
	u32 TextureOffset;
	
	u32 And3, Shift3, Shift4;
	
	u32 PixelsPerLine;
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	
	
	/*
	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];
	
	// not used
	//u32 TexBPP;
	//u32 TexWidth_Shift;


	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	
	// z-buffer
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;


	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;

	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	
	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;
	*/

	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;
	
#ifdef ENABLE_MIPMAP
	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	MIPTBP1_t *MIPTBP1 = &GPURegsGp.MIPTBP1_1;
	MIPTBP2_t *MIPTBP2 = &GPURegsGp.MIPTBP2_1;
#endif
	
	/*
	///////////////////////////////////
	// put top coordinates in x0,y0
	VRAM32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	VRAM16 = (u16*) VRAM32;
	
	buf32 = VRAM32;
	buf16 = VRAM16;
	*/
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;


	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	// if TBW is 1, then use TexWidth ??
	//if ( TEX0 [ Ctx ].TBW0 == 1 ) TexBufWidth = TexWidth;
	
	
	//TexWidth_Shift = TEX0 [ Ctx ].TW;
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;

#ifdef ENABLE_MIPMAP
	s32 iLevel;
	float fLOD [ 3 ];
	float fRQ;
	float fK, fL;
	s32 lK;
	s32 lL;
	float fMinLOD;

	// check if MXL (maximum LOD level is zero or not)
	if ( TEX1 [ Ctx ].MXL )
	{
		// get K
		lK = TEX1 [ Ctx ].K;

		// extend sign
		lK = ( lK << 20 ) >> 20;

		// check whether level is set by K
		if ( TEX1 [ Ctx ].LCM )
		{
			//LOD = K
			iLevel = lK >> 4;

		}
		else
		{
			// LOD = (log2(1/|Q|)<<L)+K

			// convert to float
			fK = (float) lK;

			// the lower 4 bits are the fractional part of K
			fK = fK / 16.0;

			// get L
			lL = TEX1 [ Ctx ].L;
			lL = 1 << lL;

			fL = (float) lL;

			// get lod0
			fRQ = 1 / fQ0;
			fLOD[0] = ( log2f( abs(fRQ) ) * fL ) + fK;
			fRQ = 1 / fQ1;
			fLOD[1] = ( log2f( abs(fRQ) ) * fL ) + fK;
			fRQ = 1 / fQ2;
			fLOD[2] = ( log2f( abs(fRQ) ) * fL ) + fK;

			// get max LOD (just need to fix bugs for now)
			fMinLOD = ( fLOD[0] < fLOD[1] ) ? fLOD[0] : fLOD[1];
			fMinLOD = ( fMinLOD < fLOD[2] ) ? fMinLOD : fLOD[2];

			// get the level to use for texture
			iLevel = (s32) fMinLOD;
		}

		if ( iLevel <= 0 )
		{
			iLevel = 0;
		}
		else if ( iLevel > TEX1 [ Ctx ].MXL )
		{
			iLevel = TEX1 [ Ctx ].MXL;
		}

		// also, make sure level is not greater than 6
		if ( iLevel > 6 )
		{
			iLevel = 6;
		}

		// get texture pointer for that level
		switch ( iLevel )
		{
			// level 0 was set above
			case 1:
				TexBufStartOffset32 = MIPTBP1 [ Ctx ].TBP1 << 6;
				TexBufWidth = MIPTBP1 [ Ctx ].TBW1 << 6;
				break;

			case 2:
				TexBufStartOffset32 = MIPTBP1 [ Ctx ].TBP2 << 6;
				TexBufWidth = MIPTBP1 [ Ctx ].TBW2 << 6;
				break;

			case 3:
				TexBufStartOffset32 = MIPTBP1 [ Ctx ].TBP3 << 6;
				TexBufWidth = MIPTBP1 [ Ctx ].TBW1 << 3;
				break;

			case 4:
				TexBufStartOffset32 = MIPTBP2 [ Ctx ].TBP4 << 6;
				TexBufWidth = MIPTBP2 [ Ctx ].TBW4 << 6;
				break;

			case 5:
				TexBufStartOffset32 = MIPTBP2 [ Ctx ].TBP5 << 6;
				TexBufWidth = MIPTBP2 [ Ctx ].TBW5 << 6;
				break;

			case 6:
				TexBufStartOffset32 = MIPTBP2 [ Ctx ].TBP6 << 6;
				TexBufWidth = MIPTBP2 [ Ctx ].TBW6 << 6;
				break;
		}

		TexWidth >>= iLevel;
		TexHeight >>= iLevel;
		if ( TexWidth )
		{
			TexWidth_Mask = TexWidth - 1;
		}
		else
		{
			TexWidth = 1;
			TexWidth_Mask = 1;
		}

		if ( TexHeight )
		{
			TexHeight_Mask = TexHeight - 1;
		}
		else
		{
			TexHeight = 1;
			TexHeight_Mask = 1;
		}
	}
#endif


	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	CLUTOffset = TEX0 [ Ctx ].CSA;


	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	ptr_texture32 = (u32*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture8 = (u8*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	// assume bpp = 32, ptr = 32-bit
	Shift1 = 0;
	
	// check if the texture pixel format is bgr24 or bgr32
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	
	
	/*
	if ( PixelFormat == 0x14 )
	{
		// bpp = 4, ptr = 32-bit
		Shift1 = 3;
		Shift2 = 2;
		And1 = 0x7; And2 = 0xf;
	}
	
	if ( PixelFormat == 0x13 )
	{
		// bpp = 8, ptr = 32-bit
		Shift1 = 2;
		Shift2 = 3;
		And1 = 0x3; And2 = 0xff;
	}
	
	if ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) )
	{
		// bpp = 16, texture ptr = 32-bit
		Shift1 = 1;
		Shift2 = 4;
		And1 = 0x1; And2 = 0xffff;
	}
	
	
	// check for psmt8h
	if ( PixelFormat == 0x1b )
	{
		// texture pixel format is psmt8h //
		
		Shift0 = 24;
		And2 = 0xff000000;
		
	}


	// check for psmt4hl
	if ( PixelFormat == 0x24 )
	{
		// texture pixel format is psmt4hl //
		
		Shift0 = 24;
		And2 = 0x0f000000;
		
	}

	// check for psmt4hh
	if ( PixelFormat == 0x2c )
	{
		// texture pixel format is psmt4hh //
		
		Shift0 = 28;
		And2 = 0xf0000000;
		
	}
	*/
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			break;
			
		// PSMCT24
		case 1:
			break;
			
		// PSMCT16
		case 2:
			break;
			
		// PSMCT16S
		case 0xa:
			break;
			
		// PSMT8
		case 0x13:
			break;
			
		// PSMT4
		case 0x14:
			break;
			
		// PSMT8H
		case 0x1b:
			break;
			
		// PSMT4HL
		case 0x24:
			break;
			
		// PSMT4HH
		case 0x2c:
			break;
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
			break;
			
		// PSMZ16
		case 0x32:
			break;
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	/*
	// assume clut bpp = 24/32, 32-bit ptr
	And3 = 0; Shift4 = 0;
	Shift3 = 0;
	
	if ( ( CLUTPixelFormat == 0x2 ) || ( CLUTPixelFormat == 0xa ) )
	{
		// bpp = 16, ptr = 32-bit
		And3 = 1;
		Shift4 = 4;

		Shift3 = 1;
		
		//if ( FrameBuffer_PixelFormat == 0 || FrameBuffer_PixelFormat == 1 )
		//{
		//	Pixel_SrcMask = 0x1f;
		//	Pixel_SrcBpp = 5;
		//	Pixel_DstShift1 = 3;
		//}
	}
	*/
	
	// important: reading from buffer in 32-bit units, so the texture buffer width should be divided accordingly
	//TexBufWidth >>= Shift1;
	
	
	// get the shade color
	//color_add = bgr;
	c1 = GPURegsGp.RGBAQ.R;
	c2 = GPURegsGp.RGBAQ.G;
	c3 = GPURegsGp.RGBAQ.B;
	ca = GPURegsGp.RGBAQ.A;


	if ( !CLUTStoreMode )
	{
		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
	}
	else
	{
		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}

	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// invert z for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
	z2 ^= 0xffffffffULL;
#endif

	// get fog
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	f2 = (u64) f [ Coord2 ].F;

	// get S
	fS0 = st [ Coord0 ].fS;
	fS1 = st [ Coord1 ].fS;
	fS2 = st [ Coord2 ].fS;
	
	// get T
	fT0 = st [ Coord0 ].fT;
	fT1 = st [ Coord1 ].fT;
	fT2 = st [ Coord2 ].fT;
	
	// get Q
	fQ0 = rgbaq [ Coord0 ].fQ;
	fQ1 = rgbaq [ Coord1 ].fQ;
	fQ2 = rgbaq [ Coord2 ].fQ;
	
	// prepare S,T,Q values
	fS0 = fS0 * ( TexWidth );
	fS1 = fS1 * ( TexWidth );
	fS2 = fS2 * ( TexWidth );
	
	fT0 = fT0 * ( TexHeight );
	fT1 = fT1 * ( TexHeight );
	fT2 = fT2 * ( TexHeight );
	
	//fQ0 = 1.0f / fQ0;
	//fQ1 = 1.0f / fQ1;
	//fQ2 = 1.0f / fQ2;
	
	
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
		// means using uv coords when FST is set //
		
		uv_temp [ Coord0 ].U = uv [ Coord0 ].U;
		uv_temp [ Coord0 ].V = uv [ Coord0 ].V;
		uv_temp [ Coord1 ].U = uv [ Coord1 ].U;
		uv_temp [ Coord1 ].V = uv [ Coord1 ].V;
		uv_temp [ Coord2 ].U = uv [ Coord2 ].U;
		uv_temp [ Coord2 ].V = uv [ Coord2 ].V;
		
		// if using u,v coords, then the larger point should be minus one
		//if ( u1 > u0 ) u1 -= ( 1 << 4 ); else u0 -= ( 1 << 4 );
		//if ( v1 > v0 ) v1 -= ( 1 << 4 ); else v0 -= ( 1 << 4 );
	}
	else
	{
		// ST coords for triangles not implemented yet //

//#ifdef VERBOSE_TRIANGLE_ST
//		cout << "\nhps2x64: ALERT: GPU: ST coords for triangles not implemented yet!!!\n";
//#endif

#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; STCoords: s0=" << st [ Coord0 ].fS << " t0=" << st [ Coord0 ].fT << " s1=" << st [ Coord1 ].fS << " t1=" << st [ Coord1 ].fT << " s2=" << st [ Coord2 ].fS << " t2=" << st [ Coord2 ].fT;
#endif

		// note: texture width/height should probably be minus one
		uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		//uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT ) * ( (float) TexHeight ) * 16.0f );
		
	}

	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
	debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	// coords are in 12.4 fixed point
	//x0 >>= 4;
	//y0 >>= 4;
	//x1 >>= 4;
	//y1 >>= 4;
	//x2 >>= 4;
	//y2 >>= 4;
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleTexture";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		//debug << " MXL=" << TEX1 [ Ctx ].MXL;
		//debug << " LCM=" << TEX1 [ Ctx ].LCM;
		//debug << " K=" << TEX1 [ Ctx ].K;
		debug << " FIX=" << (u32) ALPHA_X.FIX;
		debug << " A=" << (u32) uA;
		debug << " B=" << (u32) uB;
		debug << " C=" << (u32) uC;
		debug << " D=" << (u32) uD;
		debug << " TFX=" << (u32) TEXX [ Ctx ].TFX;
		debug << " TCC=" << (u32) TEXX [ Ctx ].TCC;
		debug << hex << " ZBuf=" << (ZBufferStartOffset32 >> 11);
		debug << PixelFormat_Names [ ZBuffer_PixelFormat ];
		debug << " ZFlags=" << ZBUF_X.Value;
		debug << " TEST=" << TEST_X.Value;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//dudx = ( ( ( ( (s64) ( uv [ Coord0 ].U - uv [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].U - uv [ Coord2 ].U ) ) * t1 ) ) << 12 ) / denominator;
		//dvdx = ( ( ( ( (s64) ( uv [ Coord0 ].V - uv [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].V - uv [ Coord2 ].V ) ) * t1 ) ) << 12 ) / denominator;
		dudx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].U - uv_temp [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].U - uv_temp [ Coord2 ].U ) ) * t1 ) ) << 16 ) / denominator;
		dvdx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].V - uv_temp [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].V - uv_temp [ Coord2 ].V ) ) * t1 ) ) << 16 ) / denominator;
	
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;

		dfdx = ( ( ( ( (s64) ( f0 - f2 ) ) * t0 ) - ( ( (s64) ( f1 - f2 ) ) * t1 ) ) << 28 ) / denominator;
		
		dSdx = ( ( ( fS0 - fS2 ) * ( t0 / 16.0f ) ) - ( ( fS1 - fS2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dTdx = ( ( ( fT0 - fT2 ) * ( t0 / 16.0f ) ) - ( ( fT1 - fT2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dQdx = ( ( ( fQ0 - fQ2 ) * ( t0 / 16.0f ) ) - ( ( fQ1 - fQ2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		
		
		//drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << 24 ) / denominator;
		//dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << 24 ) / denominator;
		//dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << 24 ) / denominator;
		//dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << 24 ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	//u [ X0Index ] = ( uv [ Coord0 ].U << 12 );
	//v [ X0Index ] = ( uv [ Coord0 ].V << 12 );
	u [ X0Index ] = ( uv_temp [ Coord0 ].U << 12 );
	v [ X0Index ] = ( uv_temp [ Coord0 ].V << 12 );
	
	z [ X0Index ] = ( ((s64)z0) << 23 );
	fogc [ X0Index ] = ( ((s64)f0) << 24 );
	
	fS [ X0Index ] = fS0;
	fT [ X0Index ] = fT0;
	fQ [ X0Index ] = fQ0;
	
	//r [ X0Index ] = ( rgbaq [ Coord0 ].R << 24 );
	//g [ X0Index ] = ( rgbaq [ Coord0 ].G << 24 );
	//b [ X0Index ] = ( rgbaq [ Coord0 ].B << 24 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		u [ X1Index ] = u [ X0Index ];
		v [ X1Index ] = v [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		fogc [ X1Index ] = fogc [ X0Index ];
		
		fS [ X1Index ] = fS [ X0Index ];
		fT [ X1Index ] = fT [ X0Index ];
		fQ [ X1Index ] = fQ [ X0Index ];
		
		//r [ X1Index ] = r [ X0Index ];
		//g [ X1Index ] = g [ X0Index ];
		//b [ X1Index ] = b [ X0Index ];
		
		//dx_left = (((s64)( x1 - x0 )) << 32 ) / ((s64)( y1 - y0 ));
		//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		//dudy [ X1Index ] = (((s32)( uv [ Coord1 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y1 - y0 ));
		//dvdy [ X1Index ] = (((s32)( uv [ Coord1 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y1 - y0 ));
		dudy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y1 - y0 ));
		dvdy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y1 - y0 ));
		
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		dfdy [ X1Index ] = (((s64)( f1 - f0 )) << 28 ) / ((s64)( y1 - y0 ));
		
		//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
		//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
		dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
		dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
		
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 28 ) / ((s64)( y2 - y0 ));
		
		dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
		dSdy [ X1Index ] = ( fS1 - fS0 ) / ( ( y1 - y0 ) / 16.0f );
		
		dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
		dTdy [ X1Index ] = ( fT1 - fT0 ) / ( ( y1 - y0 ) / 16.0f );
		
		dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ1 - fQ0 ) / ( ( y1 - y0 ) / 16.0f );
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
		//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
		u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
		v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );
		
		z [ X1Index ] = ( ((s64)z1) << 23 );
		fogc [ X1Index ] = ( ((s64)f1) << 24 );
		
		fS [ X1Index ] = fS1;
		fT [ X1Index ] = fT1;
		fQ [ X1Index ] = fQ1;
		
		//r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
		//g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
		//b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
			//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
			dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
			dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
			
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 28 ) / ((s64)( y2 - y0 ));
			
			dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
			dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
			dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
			
			// only need to set dr,dg,db for the x0/x2 side here
			//drdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 24 ) / ((s32)( y2 - y0 ));
			//dgdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 24 ) / ((s32)( y2 - y0 ));
			//dbdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 24 ) / ((s32)( y2 - y0 ));
		}
	}
	
	
	
	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}
	
	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	//StartY = y0;
	//EndY = y1;

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
	v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
	
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
	
	fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
	fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
	fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
	
	
	//u [ 1 ] += ( dudy [ 1 ] >> 4 ) * Temp;
	//v [ 1 ] += ( dvdy [ 1 ] >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}


	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;

		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		

		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );

			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/*
			//DitherLine = & ( DitherArray [ ( Line & 0x3 ) << 2 ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			
#ifdef DEBUG_TEST
	debug << "\r\n";
#endif


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				TexCoordX = ( iU >> 16 ) /* & TexWidth_Mask */;
				TexCoordY = ( iV >> 16 ) /* & TexHeight_Mask */;

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					TexCoordX = (long) (iS / iQ);
					TexCoordY = (long) (iT / iQ);
				}
#endif

				TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				TexCoordY &= TexY_And;
				TexCoordY |= TexY_Or;

				TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				TexCoordX &= TexX_And;
				TexCoordX |= TexX_Or;

//if ( Clamp_ModeX == 1 && TexWidth == 64 )
//{
//	debug << "\r\nTexCoordX=" << dec << TexCoordX;
//	debug << " TexCoordY=" << dec << TexCoordY;
//}
				


				// get texel from texture buffer (32-bit frame buffer) //

				//bgr = ptr_texture [ ( TexCoordX >> Shift1 ) + ( TexCoordY * ( TexBufWidth >> Shift1 ) ) ];
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT24
					case 1:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16
					case 2:
						bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16S
					case 0xa:
						bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT8
					case 0x13:
						bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT4
					case 0x14:
						bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						bgr_temp = ( ptr_texture8 [ bgr >> 1 ] >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						break;
						
					// PSMT8H
					case 0x1b:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						break;
						
					// PSMT4HL
					case 0x24:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
						bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16
					case 0x32:
						bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16S
					case 0x3a:
						bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
				}
				
				// look up color value in CLUT if needed //
				//if ( PixelFormat >> 4 )
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					bgr = ptr_clut16 [ bgr_temp ];
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
					}
				}
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// pixel is probably not transparent ?? //
						
						bgr_temp = bgr;
						
						// convert to 32-bit
						bgr = ConvertPixel16To24 ( bgr );
						
						// set the texture alpha for 16 bit pixel
						bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					}
					/*
					else if ( bgr )
					{
						// set the texture alpha for 16 bit pixel
						bgr |= GPURegsGp.TEXA.TA0 << 24;
					}
					*/
				} // end if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )

				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					bgr &= 0xffffff;
					
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// alpha is TA0
						bgr |= ( GPURegsGp.TEXA.TA0 << 24 );
					}
#ifdef ENABLE_TRANSPARENT24
					else
					{
						// if alpha is not TA0, then it is transparent??
						iU += dudx;
						iV += dvdx;
						
						iZ += dzdx;
						iF += dfdx;
						
						iS += dSdx;
						iT += dTdx;
						iQ += dQdx;
						
						continue;
					}
#endif
				}

/*				
debug << " x=" << dec << x_across;
//debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 0 );
debug << " y=" << dec << Line;
debug << " z=" << dec << ( iZ >> 23 );
debug << " tx=" << dec << TexCoordX;
debug << " ty=" << dec << TexCoordY;
debug << " bgr=" << hex << bgr;
debug << " texbufwidth=" << dec << TexBufWidth;
//debug << " texoffset=" << dec << TexOffset32;
//debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 0 );
*/
				
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				
						
				iU += dudx;
				iV += dvdx;
				
				iZ += dzdx;
				iF += dfdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
					//iR += drdx;
					//iG += dgdx;
					//iB += dbdx;
				
			}
			
		}
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		//x_left += dx_left;
		//x_right += dx_right;
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		//u [ 1 ] += dudy [ 1 ];
		//v [ 1 ] += dvdy [ 1 ];
		
		//r [ 0 ] += drdy [ 0 ];
		//g [ 0 ] += dgdy [ 0 ];
		//b [ 0 ] += dbdy [ 0 ];
		//r [ 1 ] += drdy [ 1 ];
		//g [ 1 ] += dgdy [ 1 ];
		//b [ 1 ] += dbdy [ 1 ];
	}
	
	} // if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
	//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
	u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
	v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );

	fS [ X1Index ] = fS1;
	fT [ X1Index ] = fT1;
	fQ [ X1Index ] = fQ1;
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	fogc [ X1Index ] = ( ((s64)f1) << 24 );
	
	//r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
	//g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
	//b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//dudy [ X1Index ] = (((s64)( uv [ Coord2 ].U - uv [ Coord1 ].U )) << 12 ) / ((s64)( y2 - y1 ));
		//dvdy [ X1Index ] = (((s64)( uv [ Coord2 ].V - uv [ Coord1 ].V )) << 12 ) / ((s64)( y2 - y1 ));
		dudy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].U - uv_temp [ Coord1 ].U )) << 16 ) / ((s64)( y2 - y1 ));
		dvdy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].V - uv_temp [ Coord1 ].V )) << 16 ) / ((s64)( y2 - y1 ));
		
		dSdy [ X1Index ] = ( fS2 - fS1 ) / ( ( y2 - y1 ) / 16.0f );
		dTdy [ X1Index ] = ( fT2 - fT1 ) / ( ( y2 - y1 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ2 - fQ1 ) / ( ( y2 - y1 ) / 16.0f );
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
		dfdy [ X1Index ] = (((s64)( f2 - f1 )) << 28 ) / ((s64)( y2 - y1 ));
		
		//drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 24 ) / ((s64)( y2 - y1 ));
		//dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 24 ) / ((s64)( y2 - y1 ));
		//dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 24 ) / ((s64)( y2 - y1 ));
	}
	
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	
	// the line starts at y1 from here
	//Line = y1;

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	u [ X1Index ] += ( dudy [ X1Index ] >> 4 ) * Temp;
	v [ X1Index ] += ( dvdy [ X1Index ] >> 4 ) * Temp;

	fS [ X1Index ] += ( dSdy [ X1Index ] ) * ( Temp / 16.0f );
	fT [ X1Index ] += ( dTdy [ X1Index ] ) * ( Temp / 16.0f );
	fQ [ X1Index ] += ( dQdy [ X1Index ] ) * ( Temp / 16.0f );
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	fogc [ X1Index ] += ( dfdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
		v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
		
		fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
		fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
		fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
		
		//u [ 1 ] += ( dudy [ 1 ] >> 4 ) * Temp;
		//v [ 1 ] += ( dvdy [ 1 ] >> 4 ) * Temp;
	}
	

	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}



	
	// ***testing*** not sure about case where triangle is only one pixel high??
	//if ( ( ( y2 - y0 ) < 0x10 ) && ( EndY < StartY ) ) EndY++;
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;
		
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		//if ( EndX < StartX ) EndX = StartX;
		
		// ***testing*** unsure of case where it is less than one pixel wide
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
		
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/*
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			//DitherLine = & ( DitherArray [ ( Line & 0x3 ) << 2 ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				TexCoordX = ( iU >> 16 ) /* & TexWidth_Mask */;
				TexCoordY = ( iV >> 16 ) /* & TexHeight_Mask */;
				
#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					TexCoordX = (long) (iS / iQ);
					TexCoordY = (long) (iT / iQ);
				}
#endif
				
				TexCoordY = ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY;
				TexCoordY = ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY;
				TexCoordY &= TexY_And;
				TexCoordY |= TexY_Or;

				TexCoordX = ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX;
				TexCoordX = ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX;
				TexCoordX &= TexX_And;
				TexCoordX |= TexX_Or;
				
//if ( Clamp_ModeX == 1 && TexWidth == 64 )
//{
//	debug << "\r\nTexCoordX=" << dec << TexCoordX;
//	debug << " TexCoordY=" << dec << TexCoordY;
//}
				
				// get texel from texture buffer (32-bit frame buffer) //

				//bgr = ptr_texture [ ( TexCoordX >> Shift1 ) + ( TexCoordY * ( TexBufWidth >> Shift1 ) ) ];
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT24
					case 1:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16
					case 2:
						bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16S
					case 0xa:
						bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT8
					case 0x13:
						bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT4
					case 0x14:
						bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						bgr_temp = ( ptr_texture8 [ bgr >> 1 ] >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						break;
						
					// PSMT8H
					case 0x1b:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						break;
						
					// PSMT4HL
					case 0x24:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
						bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16
					case 0x32:
						bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16S
					case 0x3a:
						bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
				}
				
				// look up color value in CLUT if needed //
				//if ( PixelFormat >> 4 )
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					bgr = ptr_clut16 [ bgr_temp ];
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
					}
				}
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// pixel is probably not transparent ?? //
						
						bgr_temp = bgr;
						
						// convert to 32-bit
						bgr = ConvertPixel16To24 ( bgr );
						
						// set the texture alpha for 16 bit pixel
						bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					}
					/*
					else if ( bgr )
					{
						// set the texture alpha for 16 bit pixel
						bgr |= GPURegsGp.TEXA.TA0 << 24;
					}
					*/
				}
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					bgr &= 0xffffff;
					
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// alpha is TA0
						bgr |= ( GPURegsGp.TEXA.TA0 << 24 );
					}
#ifdef ENABLE_TRANSPARENT24
					else
					{
						// if alpha is not TA0, then it is transparent??
						iU += dudx;
						iV += dvdx;
						
						iZ += dzdx;
						iF += dfdx;
						
						iS += dSdx;
						iT += dTdx;
						iQ += dQdx;
						
						continue;
					}
#endif
				}
				
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				
				
				iU += dudx;
				iV += dvdx;
				
				iZ += dzdx;
				iF += dfdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				//iR += drdx;
				//iG += dgdx;
				//iB += dbdx;
				
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		//x_left += dx_left;
		//x_right += dx_right;
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		//u [ 1 ] += dudy [ 1 ];
		//v [ 1 ] += dvdy [ 1 ];
		
		//r [ 0 ] += drdy [ 0 ];
		//g [ 0 ] += dgdy [ 0 ];
		//b [ 0 ] += dbdy [ 0 ];
		//r [ 1 ] += drdy [ 1 ];
		//g [ 1 ] += dgdy [ 1 ];
		//b [ 1 ] += dbdy [ 1 ];
	}
	
	} // if ( EndY >= StartY )

}


#ifdef _ENABLE_SSE41_TRIANGLE_TEXTURE

void GPU::DrawTriangle_Texture32_DSx4 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 4;

	
	u16 *CLUT_LUT;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;
	
	u32 *ptr_texture32;
	u16 *ptr_texture16;
	u8 *ptr_texture8;
	
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	
	float fS0, fS1, fS2, fT0, fT1, fT2, fQ0, fQ1, fQ2;
	float fS [ 2 ], fT [ 2 ], fQ [ 2 ], dSdy [ 2 ], dTdy [ 2 ], dQdy [ 2 ];
	float iS, iT, iQ, dSdx, dTdx, dQdx;
	
	s64 iU, iV;
	
	//s32 iR, iG, iB;
	
	s64 u [ 2 ], v [ 2 ];
	s64 dudy [ 2 ], dvdy [ 2 ];
	
	//u32 r [ 2 ], g [ 2 ], b [ 2 ];
	//s32 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	
	s64 dudx, dvdx;
	//s32 drdx, dgdx, dbdx;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;

	// fog coords ?
	s64 f0, f1, f2;
	s64 fogc [ 2 ], dfdy [ 2 ];
	s64 dfdx;
	s64 iF;
	
	u32 c1, c2, c3, ca;
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	//u32 DestPixel, PixelMask = 0, SetPixelMask = 0;


	s32 TexCoordX, TexCoordY;
	u32 TextureOffset;
	
	u32 And3, Shift3, Shift4;
	
	u32 PixelsPerLine;
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	
	

	__m128i vEnable;
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 ( 3, 2, 1, 0 );

	__m128 vIdentityf;
	vIdentityf = _mm_set_ps ( 3.0f, 2.0f, 1.0f, 0.0f );
	
	__m128i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m128i vXInc;
	__m128i vYInc;
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vYInc = _mm_set1_epi32 ( 1 );
	
	__m128i vZInc;
	__m128i vZIdentity0, vZIdentity1;
	__m128i vZacross0, vZacross1;
	
	__m128i vFInc;
	__m128i vFIdentity;
	__m128i vFacross;
	
	__m128i vUInc, vVInc;
	__m128i vUIdentity, vVIdentity;
	__m128i vUacross, vVacross;
	
	__m128 vSInc, vTInc, vQInc;
	__m128 vSIdentity, vTIdentity, vQIdentity;
	__m128 vSacross, vTacross, vQacross;
	
	__m128i vTexOffset32, vTexBufWidth;
	__m128i vTexCoordY, vTexY_Min, vTexY_Max, vTexY_And, vTexY_Or;
	__m128i vTexCoordX, vTexX_Min, vTexX_Max, vTexX_And, vTexX_Or;
	__m128i vRed, vGreen, vBlue, vAlpha;
	__m128i vBgr_temp, vBgr2, vca, vc1, vc2, vc3;
	
	
	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;
	
	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;


	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	CLUTOffset = TEX0 [ Ctx ].CSA;


	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	ptr_texture32 = (u32*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture8 = (u8*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	
	// check if the texture pixel format is bgr24 or bgr32
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	
	
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			break;
			
		// PSMCT24
		case 1:
			break;
			
		// PSMCT16
		case 2:
			break;
			
		// PSMCT16S
		case 0xa:
			break;
			
		// PSMT8
		case 0x13:
			break;
			
		// PSMT4
		case 0x14:
			break;
			
		// PSMT8H
		case 0x1b:
			break;
			
		// PSMT4HL
		case 0x24:
			break;
			
		// PSMT4HH
		case 0x2c:
			break;
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
			break;
			
		// PSMZ16
		case 0x32:
			break;
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	
	
	// get the shade color
	//color_add = bgr;
	c1 = GPURegsGp.RGBAQ.R;
	c2 = GPURegsGp.RGBAQ.G;
	c3 = GPURegsGp.RGBAQ.B;
	ca = GPURegsGp.RGBAQ.A;


	if ( !CLUTStoreMode )
	{
		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
	}
	else
	{
		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}

	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	

	// get fog
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	f2 = (u64) f [ Coord2 ].F;

	// get S
	fS0 = st [ Coord0 ].fS;
	fS1 = st [ Coord1 ].fS;
	fS2 = st [ Coord2 ].fS;
	
	// get T
	fT0 = st [ Coord0 ].fT;
	fT1 = st [ Coord1 ].fT;
	fT2 = st [ Coord2 ].fT;
	
	// get Q
	fQ0 = rgbaq [ Coord0 ].fQ;
	fQ1 = rgbaq [ Coord1 ].fQ;
	fQ2 = rgbaq [ Coord2 ].fQ;
	
	// prepare S,T,Q values
	fS0 = fS0 * ( TexWidth );
	fS1 = fS1 * ( TexWidth );
	fS2 = fS2 * ( TexWidth );
	
	fT0 = fT0 * ( TexHeight );
	fT1 = fT1 * ( TexHeight );
	fT2 = fT2 * ( TexHeight );
	
	
	
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
		// means using uv coords when FST is set //
		
		uv_temp [ Coord0 ].U = uv [ Coord0 ].U;
		uv_temp [ Coord0 ].V = uv [ Coord0 ].V;
		uv_temp [ Coord1 ].U = uv [ Coord1 ].U;
		uv_temp [ Coord1 ].V = uv [ Coord1 ].V;
		uv_temp [ Coord2 ].U = uv [ Coord2 ].U;
		uv_temp [ Coord2 ].V = uv [ Coord2 ].V;
		
	}
	else
	{
		// ST coords for triangles not implemented yet //

//#ifdef VERBOSE_TRIANGLE_ST
//		cout << "\nhps2x64: ALERT: GPU: ST coords for triangles not implemented yet!!!\n";
//#endif

#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; STCoords: s0=" << st [ Coord0 ].fS << " t0=" << st [ Coord0 ].fT << " s1=" << st [ Coord1 ].fS << " t1=" << st [ Coord1 ].fT << " s2=" << st [ Coord2 ].fS << " t2=" << st [ Coord2 ].fT;
#endif

		// note: texture width/height should probably be minus one
		uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		//uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT ) * ( (float) TexHeight ) * 16.0f );
		
	}

	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
	debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleTexture";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//dudx = ( ( ( ( (s64) ( uv [ Coord0 ].U - uv [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].U - uv [ Coord2 ].U ) ) * t1 ) ) << 12 ) / denominator;
		//dvdx = ( ( ( ( (s64) ( uv [ Coord0 ].V - uv [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].V - uv [ Coord2 ].V ) ) * t1 ) ) << 12 ) / denominator;
		dudx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].U - uv_temp [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].U - uv_temp [ Coord2 ].U ) ) * t1 ) ) << 16 ) / denominator;
		dvdx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].V - uv_temp [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].V - uv_temp [ Coord2 ].V ) ) * t1 ) ) << 16 ) / denominator;
	
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;

		dfdx = ( ( ( ( (s64) ( f0 - f2 ) ) * t0 ) - ( ( (s64) ( f1 - f2 ) ) * t1 ) ) << 20 ) / denominator;
		
		dSdx = ( ( ( fS0 - fS2 ) * ( t0 / 16.0f ) ) - ( ( fS1 - fS2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dTdx = ( ( ( fT0 - fT2 ) * ( t0 / 16.0f ) ) - ( ( fT1 - fT2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dQdx = ( ( ( fQ0 - fQ2 ) * ( t0 / 16.0f ) ) - ( ( fQ1 - fQ2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		
		
		//drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << 24 ) / denominator;
		//dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << 24 ) / denominator;
		//dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << 24 ) / denominator;
		//dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << 24 ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	//u [ X0Index ] = ( uv [ Coord0 ].U << 12 );
	//v [ X0Index ] = ( uv [ Coord0 ].V << 12 );
	u [ X0Index ] = ( uv_temp [ Coord0 ].U << 12 );
	v [ X0Index ] = ( uv_temp [ Coord0 ].V << 12 );
	
	z [ X0Index ] = ( ((s64)z0) << 23 );
	fogc [ X0Index ] = ( ((s64)f0) << 16 );
	
	fS [ X0Index ] = fS0;
	fT [ X0Index ] = fT0;
	fQ [ X0Index ] = fQ0;
	
	//r [ X0Index ] = ( rgbaq [ Coord0 ].R << 24 );
	//g [ X0Index ] = ( rgbaq [ Coord0 ].G << 24 );
	//b [ X0Index ] = ( rgbaq [ Coord0 ].B << 24 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		u [ X1Index ] = u [ X0Index ];
		v [ X1Index ] = v [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		fogc [ X1Index ] = fogc [ X0Index ];
		
		fS [ X1Index ] = fS [ X0Index ];
		fT [ X1Index ] = fT [ X0Index ];
		fQ [ X1Index ] = fQ [ X0Index ];
		
		//r [ X1Index ] = r [ X0Index ];
		//g [ X1Index ] = g [ X0Index ];
		//b [ X1Index ] = b [ X0Index ];
		
		//dx_left = (((s64)( x1 - x0 )) << 32 ) / ((s64)( y1 - y0 ));
		//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		//dudy [ X1Index ] = (((s32)( uv [ Coord1 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y1 - y0 ));
		//dvdy [ X1Index ] = (((s32)( uv [ Coord1 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y1 - y0 ));
		dudy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y1 - y0 ));
		dvdy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y1 - y0 ));
		
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		dfdy [ X1Index ] = (((s64)( f1 - f0 )) << 20 ) / ((s64)( y1 - y0 ));
		
		//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
		//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
		dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
		dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
		
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
		
		dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
		dSdy [ X1Index ] = ( fS1 - fS0 ) / ( ( y1 - y0 ) / 16.0f );
		
		dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
		dTdy [ X1Index ] = ( fT1 - fT0 ) / ( ( y1 - y0 ) / 16.0f );
		
		dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ1 - fQ0 ) / ( ( y1 - y0 ) / 16.0f );
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
		//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
		u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
		v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );
		
		z [ X1Index ] = ( ((s64)z1) << 23 );
		fogc [ X1Index ] = ( ((s64)f1) << 16 );
		
		fS [ X1Index ] = fS1;
		fT [ X1Index ] = fT1;
		fQ [ X1Index ] = fQ1;
		
		//r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
		//g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
		//b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
			//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
			dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
			dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
			
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
			
			dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
			dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
			dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
			
			// only need to set dr,dg,db for the x0/x2 side here
			//drdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 24 ) / ((s32)( y2 - y0 ));
			//dgdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 24 ) / ((s32)( y2 - y0 ));
			//dbdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 24 ) / ((s32)( y2 - y0 ));
		}
	}
	
	
	
	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}
	

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
	v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
	
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
	
	fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
	fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
	fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
	
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}


	//vAlphaSelect [ 0 ] = _mm_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm_set1_epi32 ( AlphaSelect [ 3 ] );

	
	vZIdentity0 = _mm_set_epi64x ( dzdx * 2, 0 );
	vZIdentity1 = _mm_set_epi64x ( dzdx * 3, dzdx * 1 );
	vZInc = _mm_set1_epi64x ( dzdx * ( (u64) c_iVectorSize ) );

	vTexBufWidth = _mm_set1_epi32 ( TexBufWidth );
	
	vTexY_Min = _mm_set1_epi32 ( TexY_Min );
	vTexY_Max = _mm_set1_epi32 ( TexY_Max );
	vTexY_And = _mm_set1_epi32 ( TexY_And );
	vTexY_Or = _mm_set1_epi32 ( TexY_Or );
	
	vTexX_Min = _mm_set1_epi32 ( TexX_Min );
	vTexX_Max = _mm_set1_epi32 ( TexX_Max );
	vTexX_And = _mm_set1_epi32 ( TexX_And );
	vTexX_Or = _mm_set1_epi32 ( TexX_Or );
	
	vc1 = _mm_set1_epi32 ( c1 );
	vc2 = _mm_set1_epi32 ( c2 );
	vc3 = _mm_set1_epi32 ( c3 );
	vca = _mm_set1_epi32 ( ca );

	vUInc = _mm_set1_epi32 ( dudx * c_iVectorSize );
	vVInc = _mm_set1_epi32 ( dvdx * c_iVectorSize );
	
	vFInc = _mm_set1_epi32 ( dfdx * c_iVectorSize );
	
	//vSInc = _mm_set_ps1 ( dSdx * c_iVectorSize );
	//vTInc = _mm_set_ps1 ( dTdx * c_iVectorSize );
	//vQInc = _mm_set_ps1 ( dQdx * c_iVectorSize );
	vSInc = _mm_set_ps1 ( dSdx * 4.0f );
	vTInc = _mm_set_ps1 ( dTdx * 4.0f );
	vQInc = _mm_set_ps1 ( dQdx * 4.0f );
	
	vUIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dudx ), vIdentity );
	vVIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dvdx ), vIdentity );
	
	vFIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dfdx ), vIdentity );

	vSIdentity = _mm_mul_ps ( _mm_set_ps1 ( dSdx ), vIdentityf );
	vTIdentity = _mm_mul_ps ( _mm_set_ps1 ( dTdx ), vIdentityf );
	vQIdentity = _mm_mul_ps ( _mm_set_ps1 ( dQdx ), vIdentityf );
	
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		

		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );

			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vYacross = _mm_set1_epi32 ( Line );
			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm_add_epi32 ( _mm_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm_add_epi32 ( _mm_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm_add_epi32 ( _mm_set1_epi32 ( iF ), vFIdentity );
			
			vSacross = _mm_add_ps ( _mm_set_ps1 ( iS ), vSIdentity );
			vTacross = _mm_add_ps ( _mm_set_ps1 ( iT ), vTIdentity );
			vQacross = _mm_add_ps ( _mm_set_ps1 ( iQ ), vQIdentity );

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm_cvttps_epi32 ( _mm_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm_cvttps_epi32 ( _mm_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm_and_si128 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm_or_si128 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm_and_si128 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm_or_si128 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 24 ), 24 );
#else
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 28 ), 28 );
#else
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> _mm_extract_epi32 ( vBgr, 3 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> _mm_extract_epi32 ( vBgr, 2 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> _mm_extract_epi32 ( vBgr, 1 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> _mm_extract_epi32 ( vBgr, 0 ) ) & 0xf );
#endif
						
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 24 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 );
#endif
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 4 ), 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 ) & 0xf );
#endif
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 28 );
#endif
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
#ifdef ENABLE_AVX128
					vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
					//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_clut16, vBgr_temp, 2 );
#else
					//bgr = ptr_clut16 [ bgr_temp ];
					vBgr = _mm_set_epi32 (
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) ] );
#endif
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
#ifdef ENABLE_AVX128
						vBgr2 = _mm_mask_i32gather_epi32 ( vBgr2, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						//vBgr2 = _mm_i32gather_epi32 ( (int const*) ( ptr_clut16 + 256 ), vBgr_temp, 2 );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#else
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
						vBgr2 = _mm_set_epi32 (
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) + 256 ] );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#endif
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 + 3 ) ), _mm_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm_or_si128 ( vBgr, _mm_blendv_epi8 ( vTA0, vTA1, _mm_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm_or_si128 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				
				
				// texture function ??
				vBgr = TextureFunc32x4 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vBgr = FogFunc32x4 ( vBgr, _mm_srli_epi32 ( vFacross, 16 ) );
				}
				
				
				
				vAlphaSelect [ 0 ] = vBgr;
				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );

/*
debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 0 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 0 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 0 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 0 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 0 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 0 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 0 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 0 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 0 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 0 );

debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 1 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 1 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 1 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 1 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 1 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 1 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 1 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 1 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 1 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 1 );

debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 2 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 2 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 2 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 2 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 2 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 2 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 2 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 2 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 2 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 2 );

debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 3 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 3 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 3 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 3 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 3 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 3 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 3 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 3 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 3 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 3 );
*/


				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				/*
				//AlphaSelect [ 0 ] = _mm_extract_epi32 ( vBgr, 0 );
				if ( _mm_extract_epi32 ( vEnable, 0 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 0 ), Line,
				_mm_extract_epi32 ( vZacross, 0 ), _mm_extract_epi32 ( vBgr, 0 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 0 ) >> 16 );
				//AlphaSelect [ 1 ] = _mm_extract_epi32 ( vBgr, 1 );
				if ( _mm_extract_epi32 ( vEnable, 1 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 1 ), Line,
				_mm_extract_epi32 ( vZacross, 1 ), _mm_extract_epi32 ( vBgr, 1 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 1 ) >> 16 );
				//AlphaSelect [ 2 ] = _mm_extract_epi32 ( vBgr, 2 );
				if ( _mm_extract_epi32 ( vEnable, 2 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 2 ), Line,
				_mm_extract_epi32 ( vZacross, 2 ), _mm_extract_epi32 ( vBgr, 2 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 2 ) >> 16 );
				//AlphaSelect [ 3 ] = _mm_extract_epi32 ( vBgr, 3 );
				if ( _mm_extract_epi32 ( vEnable, 3 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 3 ), Line,
				_mm_extract_epi32 ( vZacross, 3 ), _mm_extract_epi32 ( vBgr, 3 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 3 ) >> 16 );
				*/
						
				/*
				iU += dudx;
				iV += dvdx;
				
				iZ += dzdx;
				iF += dfdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				*/
				
					//iR += drdx;
					//iG += dgdx;
					//iB += dbdx;
					
				
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm_add_epi32 ( vUacross, vUInc );
				vVacross = _mm_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm_add_epi32 ( vFacross, vFInc );
				
				vSacross = _mm_add_ps ( vSacross, vSInc );
				vTacross = _mm_add_ps ( vTacross, vTInc );
				vQacross = _mm_add_ps ( vQacross, vQInc );
			}
			
		}
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		
		//r [ 0 ] += drdy [ 0 ];
		//g [ 0 ] += dgdy [ 0 ];
		//b [ 0 ] += dbdy [ 0 ];
	}
	
	} // if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
	//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
	u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
	v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );

	fS [ X1Index ] = fS1;
	fT [ X1Index ] = fT1;
	fQ [ X1Index ] = fQ1;
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	fogc [ X1Index ] = ( ((s64)f1) << 16 );
	
	//r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
	//g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
	//b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//dudy [ X1Index ] = (((s64)( uv [ Coord2 ].U - uv [ Coord1 ].U )) << 12 ) / ((s64)( y2 - y1 ));
		//dvdy [ X1Index ] = (((s64)( uv [ Coord2 ].V - uv [ Coord1 ].V )) << 12 ) / ((s64)( y2 - y1 ));
		dudy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].U - uv_temp [ Coord1 ].U )) << 16 ) / ((s64)( y2 - y1 ));
		dvdy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].V - uv_temp [ Coord1 ].V )) << 16 ) / ((s64)( y2 - y1 ));
		
		dSdy [ X1Index ] = ( fS2 - fS1 ) / ( ( y2 - y1 ) / 16.0f );
		dTdy [ X1Index ] = ( fT2 - fT1 ) / ( ( y2 - y1 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ2 - fQ1 ) / ( ( y2 - y1 ) / 16.0f );
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
		dfdy [ X1Index ] = (((s64)( f2 - f1 )) << 20 ) / ((s64)( y2 - y1 ));
		
		//drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 24 ) / ((s64)( y2 - y1 ));
		//dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 24 ) / ((s64)( y2 - y1 ));
		//dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 24 ) / ((s64)( y2 - y1 ));
	}
	
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	
	// the line starts at y1 from here
	//Line = y1;

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	u [ X1Index ] += ( dudy [ X1Index ] >> 4 ) * Temp;
	v [ X1Index ] += ( dvdy [ X1Index ] >> 4 ) * Temp;

	fS [ X1Index ] += ( dSdy [ X1Index ] ) * ( Temp / 16.0f );
	fT [ X1Index ] += ( dTdy [ X1Index ] ) * ( Temp / 16.0f );
	fQ [ X1Index ] += ( dQdy [ X1Index ] ) * ( Temp / 16.0f );
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	fogc [ X1Index ] += ( dfdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
		v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
		
		fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
		fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
		fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
		
	}
	

	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}



	
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		// ***testing*** unsure of case where it is less than one pixel wide
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
		
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;

			
			vYacross = _mm_set1_epi32 ( Line );
			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm_add_epi32 ( _mm_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm_add_epi32 ( _mm_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm_add_epi32 ( _mm_set1_epi32 ( iF ), vFIdentity );

			vSacross = _mm_add_ps ( _mm_set_ps1 ( iS ), vSIdentity );
			vTacross = _mm_add_ps ( _mm_set_ps1 ( iT ), vTIdentity );
			vQacross = _mm_add_ps ( _mm_set_ps1 ( iQ ), vQIdentity );
			
			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm_cvttps_epi32 ( _mm_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm_cvttps_epi32 ( _mm_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm_and_si128 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm_or_si128 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm_and_si128 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm_or_si128 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 24 ), 24 );
#else
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 28 ), 28 );
#else
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> _mm_extract_epi32 ( vBgr, 3 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> _mm_extract_epi32 ( vBgr, 2 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> _mm_extract_epi32 ( vBgr, 1 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> _mm_extract_epi32 ( vBgr, 0 ) ) & 0xf );
#endif
						
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 24 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 );
#endif
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 4 ), 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 ) & 0xf );
#endif
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 28 );
#endif
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
#ifdef ENABLE_AVX128
					vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
					//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_clut16, vBgr_temp, 2 );
#else
					//bgr = ptr_clut16 [ bgr_temp ];
					vBgr = _mm_set_epi32 (
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) ] );
#endif
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
#ifdef ENABLE_AVX128
						vBgr2 = _mm_mask_i32gather_epi32 ( vBgr2, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						//vBgr2 = _mm_i32gather_epi32 ( (int const*) ( ptr_clut16 + 256 ), vBgr_temp, 2 );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#else
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
						vBgr2 = _mm_set_epi32 (
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) + 256 ] );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#endif
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 + 3 ) ), _mm_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm_or_si128 ( vBgr, _mm_blendv_epi8 ( vTA0, vTA1, _mm_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm_or_si128 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				
				
				// texture function ??
				vBgr = TextureFunc32x4 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vBgr = FogFunc32x4 ( vBgr, _mm_srli_epi32 ( vFacross, 16 ) );
				}
				
				
				
				vAlphaSelect [ 0 ] = vBgr;

				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );

/*
debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 0 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 0 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 0 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 0 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 0 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 0 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 0 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 0 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 0 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 0 );
debug << " calc=" << dec << CvtAddrPix32 ( _mm_extract_epi32 ( vTexCoordX, 0 ), _mm_extract_epi32 ( vTexCoordY, 0 ), _mm_extract_epi32 ( vTexBufWidth, 0 ) );

debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 1 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 1 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 1 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 1 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 1 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 1 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 1 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 1 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 1 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 1 );
debug << " calc=" << dec << CvtAddrPix32 ( _mm_extract_epi32 ( vTexCoordX, 1 ), _mm_extract_epi32 ( vTexCoordY, 1 ), _mm_extract_epi32 ( vTexBufWidth, 1 ) );

debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 2 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 2 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 2 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 2 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 2 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 2 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 2 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 2 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 2 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 2 );
debug << " calc=" << dec << CvtAddrPix32 ( _mm_extract_epi32 ( vTexCoordX, 2 ), _mm_extract_epi32 ( vTexCoordY, 2 ), _mm_extract_epi32 ( vTexBufWidth, 2 ) );

debug << " x=" << dec << _mm_extract_epi32 ( vXacross, 3 );
debug << " xmax=" << dec << _mm_extract_epi32 ( vXmax, 3 );
debug << " y=" << dec << _mm_extract_epi32 ( vYacross, 3 );
debug << " z=" << dec << _mm_extract_epi32 ( vZacross, 3 );
debug << " tx=" << dec << _mm_extract_epi32 ( vTexCoordX, 3 );
debug << " ty=" << dec << _mm_extract_epi32 ( vTexCoordY, 3 );
debug << " bgr=" << hex << _mm_extract_epi32 ( vBgr, 3 );
debug << " texbufwidth=" << dec << _mm_extract_epi32 ( vTexBufWidth, 3 );
debug << " texoffset=" << dec << _mm_extract_epi32 ( vTexOffset32, 3 );
debug << " enable=" << dec << _mm_extract_epi32 ( vEnable, 3 );
debug << " calc=" << dec << CvtAddrPix32 ( _mm_extract_epi32 ( vTexCoordX, 3 ), _mm_extract_epi32 ( vTexCoordY, 3 ), _mm_extract_epi32 ( vTexBufWidth, 3 ) );
*/

				
				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				/*
				AlphaSelect [ 0 ] = _mm_extract_epi32 ( vBgr, 0 );
				if ( _mm_extract_epi32 ( vEnable, 0 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 0 ), Line,
				_mm_extract_epi32 ( vZacross, 0 ), _mm_extract_epi32 ( vBgr, 0 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 0 ) >> 16 );
				AlphaSelect [ 1 ] = _mm_extract_epi32 ( vBgr, 1 );
				if ( _mm_extract_epi32 ( vEnable, 1 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 1 ), Line,
				_mm_extract_epi32 ( vZacross, 1 ), _mm_extract_epi32 ( vBgr, 1 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 1 ) >> 16 );
				AlphaSelect [ 2 ] = _mm_extract_epi32 ( vBgr, 2 );
				if ( _mm_extract_epi32 ( vEnable, 2 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 2 ), Line,
				_mm_extract_epi32 ( vZacross, 2 ), _mm_extract_epi32 ( vBgr, 2 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 2 ) >> 16 );
				AlphaSelect [ 3 ] = _mm_extract_epi32 ( vBgr, 3 );
				if ( _mm_extract_epi32 ( vEnable, 3 ) ) PlotPixel_Texture ( _mm_extract_epi32 ( vXacross, 3 ), Line,
				_mm_extract_epi32 ( vZacross, 3 ), _mm_extract_epi32 ( vBgr, 3 ), c1, c2, c3, ca, _mm_extract_epi32 ( vFacross, 3 ) >> 16 );
				*/
				
				/*
				iU += dudx;
				iV += dvdx;
				
				iZ += dzdx;
				iF += dfdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				*/
				
				//iR += drdx;
				//iG += dgdx;
				//iB += dbdx;
				
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm_add_epi32 ( vUacross, vUInc );
				vVacross = _mm_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm_add_epi32 ( vFacross, vFInc );
				
				vSacross = _mm_add_ps ( vSacross, vSInc );
				vTacross = _mm_add_ps ( vTacross, vTInc );
				vQacross = _mm_add_ps ( vQacross, vQInc );
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		//r [ 0 ] += drdy [ 0 ];
		//g [ 0 ] += dgdy [ 0 ];
		//b [ 0 ] += dbdy [ 0 ];
		
	}
	
	} // if ( EndY >= StartY )

}

#endif

#ifdef _ENABLE_AVX2_TRIANGLE_TEXTURE

void GPU::DrawTriangle_Texture32_DSx8 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 8;

	
	u16 *CLUT_LUT;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;
	
	u32 *ptr_texture32;
	u16 *ptr_texture16;
	u8 *ptr_texture8;
	
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];
	
	float fS0, fS1, fS2, fT0, fT1, fT2, fQ0, fQ1, fQ2;
	float fS [ 2 ], fT [ 2 ], fQ [ 2 ], dSdy [ 2 ], dTdy [ 2 ], dQdy [ 2 ];
	float iS, iT, iQ, dSdx, dTdx, dQdx;
	
	s64 iU, iV;
	
	//s32 iR, iG, iB;
	
	s64 u [ 2 ], v [ 2 ];
	s64 dudy [ 2 ], dvdy [ 2 ];
	
	//u32 r [ 2 ], g [ 2 ], b [ 2 ];
	//s32 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	
	s64 dudx, dvdx;
	//s32 drdx, dgdx, dbdx;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;

	// fog coords ?
	s64 f0, f1, f2;
	s64 fogc [ 2 ], dfdy [ 2 ];
	s64 dfdx;
	s64 iF;
	
	u32 c1, c2, c3, ca;
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	//u32 DestPixel, PixelMask = 0, SetPixelMask = 0;


	s32 TexCoordX, TexCoordY;
	u32 TextureOffset;
	
	u32 And3, Shift3, Shift4;
	
	u32 PixelsPerLine;
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	
	

	__m256i vEnable;
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 ( 7, 6, 5, 4, 3, 2, 1, 0 );

	__m256 vIdentityf;
	vIdentityf = _mm256_set_ps ( 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f );
	
	__m256i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m256i vXInc;
	__m256i vYInc;
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vYInc = _mm256_set1_epi32 ( 1 );
	
	__m256i vZInc;
	__m256i vZIdentity0, vZIdentity1;
	__m256i vZacross0, vZacross1;
	
	__m256i vFInc;
	__m256i vFIdentity;
	__m256i vFacross;
	
	__m256i vUInc, vVInc;
	__m256i vUIdentity, vVIdentity;
	__m256i vUacross, vVacross;
	
	__m256 vSInc, vTInc, vQInc;
	__m256 vSIdentity, vTIdentity, vQIdentity;
	__m256 vSacross, vTacross, vQacross;
	
	__m256i vTexOffset32, vTexBufWidth;
	__m256i vTexCoordY, vTexY_Min, vTexY_Max, vTexY_And, vTexY_Or;
	__m256i vTexCoordX, vTexX_Min, vTexX_Max, vTexX_And, vTexX_Or;
	__m256i vRed, vGreen, vBlue, vAlpha;
	__m256i vBgr_temp, vBgr2, vca, vc1, vc2, vc3;
	__m256i vF;
	
	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;
	
	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;


	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	CLUTOffset = TEX0 [ Ctx ].CSA;


	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	ptr_texture32 = (u32*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture8 = (u8*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	
	// check if the texture pixel format is bgr24 or bgr32
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	
	
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			break;
			
		// PSMCT24
		case 1:
			break;
			
		// PSMCT16
		case 2:
			break;
			
		// PSMCT16S
		case 0xa:
			break;
			
		// PSMT8
		case 0x13:
			break;
			
		// PSMT4
		case 0x14:
			break;
			
		// PSMT8H
		case 0x1b:
			break;
			
		// PSMT4HL
		case 0x24:
			break;
			
		// PSMT4HH
		case 0x2c:
			break;
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
			break;
			
		// PSMZ16
		case 0x32:
			break;
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	
	
	// get the shade color
	//color_add = bgr;
	c1 = GPURegsGp.RGBAQ.R;
	c2 = GPURegsGp.RGBAQ.G;
	c3 = GPURegsGp.RGBAQ.B;
	ca = GPURegsGp.RGBAQ.A;


	if ( !CLUTStoreMode )
	{
		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
	}
	else
	{
		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}

	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	

	// get fog
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	f2 = (u64) f [ Coord2 ].F;

	// get S
	fS0 = st [ Coord0 ].fS;
	fS1 = st [ Coord1 ].fS;
	fS2 = st [ Coord2 ].fS;
	
	// get T
	fT0 = st [ Coord0 ].fT;
	fT1 = st [ Coord1 ].fT;
	fT2 = st [ Coord2 ].fT;
	
	// get Q
	fQ0 = rgbaq [ Coord0 ].fQ;
	fQ1 = rgbaq [ Coord1 ].fQ;
	fQ2 = rgbaq [ Coord2 ].fQ;
	
	// prepare S,T,Q values
	fS0 = fS0 * ( TexWidth );
	fS1 = fS1 * ( TexWidth );
	fS2 = fS2 * ( TexWidth );
	
	fT0 = fT0 * ( TexHeight );
	fT1 = fT1 * ( TexHeight );
	fT2 = fT2 * ( TexHeight );
	
	
	
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
		// means using uv coords when FST is set //
		
		uv_temp [ Coord0 ].U = uv [ Coord0 ].U;
		uv_temp [ Coord0 ].V = uv [ Coord0 ].V;
		uv_temp [ Coord1 ].U = uv [ Coord1 ].U;
		uv_temp [ Coord1 ].V = uv [ Coord1 ].V;
		uv_temp [ Coord2 ].U = uv [ Coord2 ].U;
		uv_temp [ Coord2 ].V = uv [ Coord2 ].V;
		
	}
	else
	{
		// ST coords for triangles not implemented yet //

//#ifdef VERBOSE_TRIANGLE_ST
//		cout << "\nhps2x64: ALERT: GPU: ST coords for triangles not implemented yet!!!\n";
//#endif

#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; STCoords: s0=" << st [ Coord0 ].fS << " t0=" << st [ Coord0 ].fT << " s1=" << st [ Coord1 ].fS << " t1=" << st [ Coord1 ].fT << " s2=" << st [ Coord2 ].fS << " t2=" << st [ Coord2 ].fT;
#endif

		// note: texture width/height should probably be minus one
		uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		//uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT ) * ( (float) TexHeight ) * 16.0f );
		
	}

	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
	debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleTexture";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " PABE=" << GPURegsGp.PABE;
		debug << " FBA=" << FBA_X;
		debug << " ZBUF=" << ZBUF_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//dudx = ( ( ( ( (s64) ( uv [ Coord0 ].U - uv [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].U - uv [ Coord2 ].U ) ) * t1 ) ) << 12 ) / denominator;
		//dvdx = ( ( ( ( (s64) ( uv [ Coord0 ].V - uv [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].V - uv [ Coord2 ].V ) ) * t1 ) ) << 12 ) / denominator;
		dudx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].U - uv_temp [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].U - uv_temp [ Coord2 ].U ) ) * t1 ) ) << 16 ) / denominator;
		dvdx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].V - uv_temp [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].V - uv_temp [ Coord2 ].V ) ) * t1 ) ) << 16 ) / denominator;
	
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;

		dfdx = ( ( ( ( (s64) ( f0 - f2 ) ) * t0 ) - ( ( (s64) ( f1 - f2 ) ) * t1 ) ) << 20 ) / denominator;
		
		dSdx = ( ( ( fS0 - fS2 ) * ( t0 / 16.0f ) ) - ( ( fS1 - fS2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dTdx = ( ( ( fT0 - fT2 ) * ( t0 / 16.0f ) ) - ( ( fT1 - fT2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dQdx = ( ( ( fQ0 - fQ2 ) * ( t0 / 16.0f ) ) - ( ( fQ1 - fQ2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		
		
		//drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << 24 ) / denominator;
		//dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << 24 ) / denominator;
		//dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << 24 ) / denominator;
		//dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << 24 ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	//u [ X0Index ] = ( uv [ Coord0 ].U << 12 );
	//v [ X0Index ] = ( uv [ Coord0 ].V << 12 );
	u [ X0Index ] = ( uv_temp [ Coord0 ].U << 12 );
	v [ X0Index ] = ( uv_temp [ Coord0 ].V << 12 );
	
	z [ X0Index ] = ( ((s64)z0) << 23 );
	fogc [ X0Index ] = ( ((s64)f0) << 16 );
	
	fS [ X0Index ] = fS0;
	fT [ X0Index ] = fT0;
	fQ [ X0Index ] = fQ0;
	
	//r [ X0Index ] = ( rgbaq [ Coord0 ].R << 24 );
	//g [ X0Index ] = ( rgbaq [ Coord0 ].G << 24 );
	//b [ X0Index ] = ( rgbaq [ Coord0 ].B << 24 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		u [ X1Index ] = u [ X0Index ];
		v [ X1Index ] = v [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		fogc [ X1Index ] = fogc [ X0Index ];
		
		fS [ X1Index ] = fS [ X0Index ];
		fT [ X1Index ] = fT [ X0Index ];
		fQ [ X1Index ] = fQ [ X0Index ];
		
		//r [ X1Index ] = r [ X0Index ];
		//g [ X1Index ] = g [ X0Index ];
		//b [ X1Index ] = b [ X0Index ];
		
		//dx_left = (((s64)( x1 - x0 )) << 32 ) / ((s64)( y1 - y0 ));
		//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		//dudy [ X1Index ] = (((s32)( uv [ Coord1 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y1 - y0 ));
		//dvdy [ X1Index ] = (((s32)( uv [ Coord1 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y1 - y0 ));
		dudy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y1 - y0 ));
		dvdy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y1 - y0 ));
		
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		dfdy [ X1Index ] = (((s64)( f1 - f0 )) << 20 ) / ((s64)( y1 - y0 ));
		
		//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
		//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
		dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
		dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
		
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
		
		dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
		dSdy [ X1Index ] = ( fS1 - fS0 ) / ( ( y1 - y0 ) / 16.0f );
		
		dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
		dTdy [ X1Index ] = ( fT1 - fT0 ) / ( ( y1 - y0 ) / 16.0f );
		
		dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ1 - fQ0 ) / ( ( y1 - y0 ) / 16.0f );
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
		//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
		u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
		v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );
		
		z [ X1Index ] = ( ((s64)z1) << 23 );
		fogc [ X1Index ] = ( ((s64)f1) << 16 );
		
		fS [ X1Index ] = fS1;
		fT [ X1Index ] = fT1;
		fQ [ X1Index ] = fQ1;
		
		//r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
		//g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
		//b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
			//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
			dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
			dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
			
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
			
			dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
			dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
			dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
			
			// only need to set dr,dg,db for the x0/x2 side here
			//drdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 24 ) / ((s32)( y2 - y0 ));
			//dgdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 24 ) / ((s32)( y2 - y0 ));
			//dbdy [ X0Index ] = (((s32)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 24 ) / ((s32)( y2 - y0 ));
		}
	}
	
	
	
	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}
	

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
	v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
	
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
	
	fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
	fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
	fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
	
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}


	//vAlphaSelect [ 0 ] = _mm256_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm256_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm256_set1_epi32 ( AlphaSelect [ 3 ] );

	
	vZIdentity0 = _mm256_set_epi64x ( dzdx * 6, dzdx * 4, dzdx * 2, 0 );
	vZIdentity1 = _mm256_set_epi64x ( dzdx * 7, dzdx * 5, dzdx * 3, dzdx * 1 );
	vZInc = _mm256_set1_epi64x ( dzdx * c_iVectorSize );

	vTexBufWidth = _mm256_set1_epi32 ( TexBufWidth );
	
	vTexY_Min = _mm256_set1_epi32 ( TexY_Min );
	vTexY_Max = _mm256_set1_epi32 ( TexY_Max );
	vTexY_And = _mm256_set1_epi32 ( TexY_And );
	vTexY_Or = _mm256_set1_epi32 ( TexY_Or );
	
	vTexX_Min = _mm256_set1_epi32 ( TexX_Min );
	vTexX_Max = _mm256_set1_epi32 ( TexX_Max );
	vTexX_And = _mm256_set1_epi32 ( TexX_And );
	vTexX_Or = _mm256_set1_epi32 ( TexX_Or );
	
	vc1 = _mm256_set1_epi32 ( c1 );
	vc2 = _mm256_set1_epi32 ( c2 );
	vc3 = _mm256_set1_epi32 ( c3 );
	vca = _mm256_set1_epi32 ( ca );

	vUInc = _mm256_set1_epi32 ( dudx * c_iVectorSize );
	vVInc = _mm256_set1_epi32 ( dvdx * c_iVectorSize );
	
	vFInc = _mm256_set1_epi32 ( dfdx * c_iVectorSize );
	
	vSInc = _mm256_set1_ps ( dSdx * c_iVectorSize );
	vTInc = _mm256_set1_ps ( dTdx * c_iVectorSize );
	vQInc = _mm256_set1_ps ( dQdx * c_iVectorSize );
	
	vUIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dudx ), vIdentity );
	vVIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dvdx ), vIdentity );
	
	vFIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dfdx ), vIdentity );

	vSIdentity = _mm256_mul_ps ( _mm256_set1_ps ( dSdx ), vIdentityf );
	vTIdentity = _mm256_mul_ps ( _mm256_set1_ps ( dTdx ), vIdentityf );
	vQIdentity = _mm256_mul_ps ( _mm256_set1_ps ( dQdx ), vIdentityf );
	
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		

		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );

			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vYacross = _mm256_set1_epi32 ( Line );
			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iF ), vFIdentity );
			
			vSacross = _mm256_add_ps ( _mm256_set1_ps ( iS ), vSIdentity );
			vTacross = _mm256_add_ps ( _mm256_set1_ps ( iT ), vTIdentity );
			vQacross = _mm256_add_ps ( _mm256_set1_ps ( iQ ), vQIdentity );

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm256_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm256_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm256_cvttps_epi32 ( _mm256_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm256_cvttps_epi32 ( _mm256_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm256_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm256_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm256_and_si256 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm256_or_si256 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm256_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm256_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm256_and_si256 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm256_or_si256 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix8x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 24 ), 24 );
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						vTexOffset32 = CvtAddrPix4x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm256_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 28 ), 28 );
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 24 );
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 4 ), 28 );
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 28 );
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					//bgr = ptr_clut16 [ bgr_temp ];
					
					vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
						
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
								
						vBgr2 = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						vBgr = _mm256_or_si256 ( vBgr, _mm256_slli_epi32 ( vBgr2, 16 ) );
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 + 3 ) ), _mm256_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm256_or_si256 ( vBgr, _mm256_blendv_epi8 ( vTA0, vTA1, _mm256_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm256_or_si256 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				
				// texture function ??
				vBgr = TextureFunc32x8 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vF = _mm256_srli_epi32 ( vFacross, 16 );
					vBgr = FogFunc32x8 ( vBgr, vF );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;


				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );

				
						
				iU += dudx;
				iV += dvdx;
				
				iZ += dzdx;
				iF += dfdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
					//iR += drdx;
					//iG += dgdx;
					//iB += dbdx;
					
				
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm256_add_epi32 ( vUacross, vUInc );
				vVacross = _mm256_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm256_add_epi32 ( vFacross, vFInc );
				
				vSacross = _mm256_add_ps ( vSacross, vSInc );
				vTacross = _mm256_add_ps ( vTacross, vTInc );
				vQacross = _mm256_add_ps ( vQacross, vQInc );
			}
			
		}
		
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		
		//r [ 0 ] += drdy [ 0 ];
		//g [ 0 ] += dgdy [ 0 ];
		//b [ 0 ] += dbdy [ 0 ];
	}
	
	} // if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
	//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
	u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
	v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );

	fS [ X1Index ] = fS1;
	fT [ X1Index ] = fT1;
	fQ [ X1Index ] = fQ1;
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	fogc [ X1Index ] = ( ((s64)f1) << 16 );
	
	//r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
	//g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
	//b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//dudy [ X1Index ] = (((s64)( uv [ Coord2 ].U - uv [ Coord1 ].U )) << 12 ) / ((s64)( y2 - y1 ));
		//dvdy [ X1Index ] = (((s64)( uv [ Coord2 ].V - uv [ Coord1 ].V )) << 12 ) / ((s64)( y2 - y1 ));
		dudy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].U - uv_temp [ Coord1 ].U )) << 16 ) / ((s64)( y2 - y1 ));
		dvdy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].V - uv_temp [ Coord1 ].V )) << 16 ) / ((s64)( y2 - y1 ));
		
		dSdy [ X1Index ] = ( fS2 - fS1 ) / ( ( y2 - y1 ) / 16.0f );
		dTdy [ X1Index ] = ( fT2 - fT1 ) / ( ( y2 - y1 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ2 - fQ1 ) / ( ( y2 - y1 ) / 16.0f );
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
		dfdy [ X1Index ] = (((s64)( f2 - f1 )) << 20 ) / ((s64)( y2 - y1 ));
		
		//drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 24 ) / ((s64)( y2 - y1 ));
		//dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 24 ) / ((s64)( y2 - y1 ));
		//dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 24 ) / ((s64)( y2 - y1 ));
	}
	
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	
	// the line starts at y1 from here
	//Line = y1;

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	u [ X1Index ] += ( dudy [ X1Index ] >> 4 ) * Temp;
	v [ X1Index ] += ( dvdy [ X1Index ] >> 4 ) * Temp;

	fS [ X1Index ] += ( dSdy [ X1Index ] ) * ( Temp / 16.0f );
	fT [ X1Index ] += ( dTdy [ X1Index ] ) * ( Temp / 16.0f );
	fQ [ X1Index ] += ( dQdy [ X1Index ] ) * ( Temp / 16.0f );
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	fogc [ X1Index ] += ( dfdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
		v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
		
		fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
		fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
		fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
		
	}
	

	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}



	
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		// ***testing*** unsure of case where it is less than one pixel wide
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
		
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;

			
			vYacross = _mm256_set1_epi32 ( Line );
			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iF ), vFIdentity );

			vSacross = _mm256_add_ps ( _mm256_set1_ps ( iS ), vSIdentity );
			vTacross = _mm256_add_ps ( _mm256_set1_ps ( iT ), vTIdentity );
			vQacross = _mm256_add_ps ( _mm256_set1_ps ( iQ ), vQIdentity );
			
			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm256_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm256_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm256_cvttps_epi32 ( _mm256_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm256_cvttps_epi32 ( _mm256_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm256_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm256_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm256_and_si256 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm256_or_si256 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm256_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm256_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm256_and_si256 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm256_or_si256 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix8x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 24 ), 24 );
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						vTexOffset32 = CvtAddrPix4x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm256_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 28 ), 28 );
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 24 );
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 4 ), 28 );
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 28 );
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					

					//bgr = ptr_clut16 [ bgr_temp ];
					
					vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
						
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
								
						vBgr2 = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						vBgr = _mm256_or_si256 ( vBgr, _mm256_slli_epi32 ( vBgr2, 16 ) );
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 + 3 ) ), _mm256_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm256_or_si256 ( vBgr, _mm256_blendv_epi8 ( vTA0, vTA1, _mm256_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm256_or_si256 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				
				// texture function ??
				vBgr = TextureFunc32x8 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vF = _mm256_srli_epi32 ( vFacross, 16 );
					vBgr = FogFunc32x8 ( vBgr, vF );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;


				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );
				
				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
				
				iU += dudx;
				iV += dvdx;
				
				iZ += dzdx;
				iF += dfdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				//iR += drdx;
				//iG += dgdx;
				//iB += dbdx;
				
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm256_add_epi32 ( vUacross, vUInc );
				vVacross = _mm256_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm256_add_epi32 ( vFacross, vFInc );
				
				vSacross = _mm256_add_ps ( vSacross, vSInc );
				vTacross = _mm256_add_ps ( vTacross, vTInc );
				vQacross = _mm256_add_ps ( vQacross, vQInc );
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		//r [ 0 ] += drdy [ 0 ];
		//g [ 0 ] += dgdy [ 0 ];
		//b [ 0 ] += dbdy [ 0 ];
		
	}
	
	} // if ( EndY >= StartY )

}

#endif


// to draw need
// ALPHA_X, CLAMP_X, COLCLAMP, DIMX, DTHE, FBA_X, FOG, FOGCOL, FRAME_X, PABE, PRIM, RGBAQ, SCANMSK, SCISSOR_X, ST, TEST_X
// TEX0_X, TEXA, TEXCLUT, UV, XYOFFSET_X, XYZ, ZBUF_X
// 23 registers
// single/double-bit registers:
// COLCLAMP, DTHE, FBA_X, PABE, SCANMSK
// 16-bit registers
// PRIM, 
// 32-bit registers
// FOGCOL, TEST_X, TEXCLUT, UV(3),
// full 64-bit registers
// CLAMP_X, FRAME_X, DIMX, RGBAQ(3), SCISSOR_X, ST(3), XYZ(3), TEX0_X
// ZBUF_X
// 15 registers
// combine into one
// PRIM, COLCLAMP, DTHE, FBA_X, PABE, SCANMSK, TEST_X, TEXCLUT
// ALPHA_X, FOGCOL
// XYOFFSET, TEXA (just shift texa to left by 16 and combine)
// UV(3), FOG(3)

// order of values
// 0: PRIM/command, COLCLAMP, DTHE, FBA_X, PABE, SCANMSK
// 1: XYOFFSET_X
// 2: FRAME_X
// 3: ZBUF_X
// 4: SCISSOR_X
// 5: ALPHA_X
// 6: TEST_X
// 7: DIMX
// 8: CLAMP_X
// 9: FOGCOL
// 10: TEX0_X
// 11: TEX1_X
// 12: MIPTBP1_X
// 13: MIPTBP2_X
// 14: TEXA
// 15: TEXCLUT
// 16: XYZ
// 17: RGBAQ
// 18: UV, FOG
// 19: ST
// 20: XYZ
// 21: RGBAQ
// 22: UV, FOG
// 23: ST
// 24: XYZ
// 25: RGBAQ
// 26: UV, FOG
// 27: ST


void GPU::DrawTriangle_GradientTexture32_DS ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 1;

	//u32 Pixel, TexelIndex,
	
	//u32 Y1_OnLeft;
	
	u16 *CLUT_LUT;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;
	
	u32 *ptr_texture32;
	u16 *ptr_texture16;
	u8 *ptr_texture8;
	
	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	//s64 Error_Left;
	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];

	float fS0, fS1, fS2, fT0, fT1, fT2, fQ0, fQ1, fQ2;
	float fS [ 2 ], fT [ 2 ], fQ [ 2 ], dSdy [ 2 ], dTdy [ 2 ], dQdy [ 2 ];
	float iS, iT, iQ, dSdx, dTdx, dQdx;
	
	s64 iU, iV;
	
	s64 iR, iG, iB;
	
	s64 u [ 2 ], v [ 2 ];
	s64 dudy [ 2 ], dvdy [ 2 ];
	
	s64 r [ 2 ], g [ 2 ], b [ 2 ];
	s64 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	
	s64 dudx, dvdx;
	s64 drdx, dgdx, dbdx;
	
	s64 a [ 2 ];
	s64 dady [ 2 ];
	s64 dadx;
	s64 iA;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;

	// fog coords ?
	s64 f0, f1, f2;
	s64 fogc [ 2 ], dfdy [ 2 ];
	s64 dfdx;
	s64 iF;
	
	//u32 c1, c2, c3, ca;
	
	/*
	union
	{
		u32 *ptr32;
		u16 *ptr16;
	};
	
	u32 *VRAM32;
	u16 *VRAM16;
	
	u32 *buf32;
	u16 *buf16;
	*/
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel, PixelMask = 0, SetPixelMask = 0;


	s32 TexCoordX, TexCoordY;
	u32 Shift1 = 0, Shift2 = 0, And1 = 0, And2 = 0;
	u32 Shift0 = 0;
	u32 TextureOffset;
	
	u32 And3, Shift3, Shift4;
	
	u32 PixelsPerLine;
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	
	
	/*
	// pabe
	u32 AlphaXor32, AlphaXor16;
	
	// alpha correction value (FBA)
	u32 PixelOr32, PixelOr16;
	
	// alpha value for rgb24 pixels
	// RGB24_Alpha -> this will be or'ed with pixel when writing to add-in alpha value for RGB24 format when writing
	// Pixel24_Mask -> this will be and'ed with pixel after reading from source
	u32 RGB24_Alpha, Pixel24_Mask;
	
	// determine if rgb24 value is transparent (a=0)
	// this will be or'ed with pixel to determine if pixels is transparent
	u32 RGB24_TAlpha;
	
	// alpha blending selection
	u32 uA, uB, uC, uD;
	
	// array for alpha selection
	u32 AlphaSelect [ 4 ];
	
	// not used
	//u32 TexBPP;
	//u32 TexWidth_Shift;


	// destination alpha test
	// DA_Enable -> the value of DATE, to be and'ed with pixel
	// DA_Test -> the value of DATM, to be xor'ed with pixel
	u32 DA_Enable, DA_Test;
	
	// source alpha test
	u64 LessThan_And, GreaterOrEqualTo_And, LessThan_Or;
	u32 Alpha_Fail;
	u32 SrcAlphaTest_Pass, SrcAlpha_ARef;
	
	// z-buffer
	u32 *zbuf32;
	u16 *zbuf16;
	u32 *zptr32;
	u16 *zptr16;
	u32 ZBuffer_Shift, ZBuffer_32bit;
	s64 ZBufValue;
	
	// depth test
	s64 DepthTest_Offset;


	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;

	// in a 24-bit frame buffer, destination alpha is always 0x80
	u32 DestAlpha24, DestMask24;
	
	// get pabe (exclusive-or this with pixel and don't perform alpha if result is 1 in MSB)
	AlphaXor32 = ( GPURegsGp.PABE & 1 ) << 31;
	AlphaXor16 = ( GPURegsGp.PABE & 1 ) << 15;
	
	// get fba (set just before drawing pixel?)
	PixelOr32 = ( FBA_X << 31 );
	PixelOr16 = ( FBA_X << 15 );
	
	// get alpha selection
	uA = ALPHA_X.A;
	uB = ALPHA_X.B;
	uC = ALPHA_X.C;
	uD = ALPHA_X.D;
	*/

	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;
	
#ifdef ENABLE_MIPMAP
	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	MIPTBP1_t *MIPTBP1 = &GPURegsGp.MIPTBP1_1;
	MIPTBP2_t *MIPTBP2 = &GPURegsGp.MIPTBP2_1;
#endif
	
	/*
	///////////////////////////////////
	// put top coordinates in x0,y0
	VRAM32 = & ( RAM32 [ FrameBufferStartOffset32 ] );
	VRAM16 = (u16*) VRAM32;
	
	buf32 = VRAM32;
	buf16 = VRAM16;
	*/
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;


	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	// if TBW is 1, then use TexWidth ??
	//if ( TEX0 [ Ctx ].TBW0 == 1 ) TexBufWidth = TexWidth;
	
	
	//TexWidth_Shift = TEX0 [ Ctx ].TW;
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;

#ifdef ENABLE_MIPMAP
	s32 iLevel;
	float fLOD [ 3 ];
	float fRQ;
	float fK, fL;
	s32 lK;
	s32 lL;
	float fMinLOD;

	// check if MXL (maximum LOD level is zero or not)
	if ( TEX1 [ Ctx ].MXL )
	{
		// get K
		lK = TEX1 [ Ctx ].K;

		// extend sign
		lK = ( lK << 20 ) >> 20;

		// check whether level is set by K
		if ( TEX1 [ Ctx ].LCM )
		{
			//LOD = K
			iLevel = lK >> 4;

		}
		else
		{
			// LOD = (log2(1/|Q|)<<L)+K

			// convert to float
			fK = (float) lK;

			// the lower 4 bits are the fractional part of K
			fK = fK / 16.0;

			// get L
			lL = TEX1 [ Ctx ].L;
			lL = 1 << lL;

			fL = (float) lL;

			// get lod0
			fRQ = 1 / fQ0;
			fLOD[0] = ( log2f( abs(fRQ) ) * fL ) + fK;
			fRQ = 1 / fQ1;
			fLOD[1] = ( log2f( abs(fRQ) ) * fL ) + fK;
			fRQ = 1 / fQ2;
			fLOD[2] = ( log2f( abs(fRQ) ) * fL ) + fK;

			// get max LOD (just need to fix bugs for now)
			fMinLOD = ( fLOD[0] < fLOD[1] ) ? fLOD[0] : fLOD[1];
			fMinLOD = ( fMinLOD < fLOD[2] ) ? fMinLOD : fLOD[2];

			// get the level to use for texture
			iLevel = (s32) fMinLOD;
		}

		if ( iLevel <= 0 )
		{
			iLevel = 0;
		}
		else if ( iLevel > TEX1 [ Ctx ].MXL )
		{
			iLevel = TEX1 [ Ctx ].MXL;
		}

		// also, make sure level is not greater than 6
		if ( iLevel > 6 )
		{
			iLevel = 6;
		}

		// get texture pointer for that level
		switch ( iLevel )
		{
			// level 0 was set above
			case 1:
				TexBufStartOffset32 = MIPTBP1 [ Ctx ].TBP1 << 6;
				TexBufWidth = MIPTBP1 [ Ctx ].TBW1 << 6;
				break;

			case 2:
				TexBufStartOffset32 = MIPTBP1 [ Ctx ].TBP2 << 6;
				TexBufWidth = MIPTBP1 [ Ctx ].TBW2 << 6;
				break;

			case 3:
				TexBufStartOffset32 = MIPTBP1 [ Ctx ].TBP3 << 6;
				TexBufWidth = MIPTBP1 [ Ctx ].TBW1 << 3;
				break;

			case 4:
				TexBufStartOffset32 = MIPTBP2 [ Ctx ].TBP4 << 6;
				TexBufWidth = MIPTBP2 [ Ctx ].TBW4 << 6;
				break;

			case 5:
				TexBufStartOffset32 = MIPTBP2 [ Ctx ].TBP5 << 6;
				TexBufWidth = MIPTBP2 [ Ctx ].TBW5 << 6;
				break;

			case 6:
				TexBufStartOffset32 = MIPTBP2 [ Ctx ].TBP6 << 6;
				TexBufWidth = MIPTBP2 [ Ctx ].TBW6 << 6;
				break;
		}

		TexWidth >>= iLevel;
		TexHeight >>= iLevel;
		if ( TexWidth )
		{
			TexWidth_Mask = TexWidth - 1;
		}
		else
		{
			TexWidth = 1;
			TexWidth_Mask = 1;
		}

		if ( TexHeight )
		{
			TexHeight_Mask = TexHeight - 1;
		}
		else
		{
			TexHeight = 1;
			TexHeight_Mask = 1;
		}
	}
#endif


	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	CLUTOffset = TEX0 [ Ctx ].CSA;


	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	ptr_texture32 = (u32*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture8 = (u8*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	// assume bpp = 32, ptr = 32-bit
	Shift1 = 0;
	
	// check if the texture pixel format is bgr24 or bgr32
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	
	
	/*
	if ( PixelFormat == 0x14 )
	{
		// bpp = 4, ptr = 32-bit
		Shift1 = 3;
		Shift2 = 2;
		And1 = 0x7; And2 = 0xf;
	}
	
	if ( PixelFormat == 0x13 )
	{
		// bpp = 8, ptr = 32-bit
		Shift1 = 2;
		Shift2 = 3;
		And1 = 0x3; And2 = 0xff;
	}
	
	if ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) )
	{
		// bpp = 16, texture ptr = 32-bit
		Shift1 = 1;
		Shift2 = 4;
		And1 = 0x1; And2 = 0xffff;
	}
	
	
	// check for psmt8h
	if ( PixelFormat == 0x1b )
	{
		// texture pixel format is psmt8h //
		
		Shift0 = 24;
		And2 = 0xff000000;
		
	}


	// check for psmt4hl
	if ( PixelFormat == 0x24 )
	{
		// texture pixel format is psmt4hl //
		
		Shift0 = 24;
		And2 = 0x0f000000;
		
	}

	// check for psmt4hh
	if ( PixelFormat == 0x2c )
	{
		// texture pixel format is psmt4hh //
		
		Shift0 = 28;
		And2 = 0xf0000000;
		
	}
	*/
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			break;
			
		// PSMCT24
		case 1:
			break;
			
		// PSMCT16
		case 2:
			break;
			
		// PSMCT16S
		case 0xa:
			break;
			
		// PSMT8
		case 0x13:
			break;
			
		// PSMT4
		case 0x14:
			break;
			
		// PSMT8H
		case 0x1b:
			break;
			
		// PSMT4HL
		case 0x24:
			break;
			
		// PSMT4HH
		case 0x2c:
			break;
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
			break;
			
		// PSMZ16
		case 0x32:
			break;
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
	debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	/*
	// assume clut bpp = 24/32, 32-bit ptr
	And3 = 0; Shift4 = 0;
	Shift3 = 0;
	
	if ( ( CLUTPixelFormat == 0x2 ) || ( CLUTPixelFormat == 0xa ) )
	{
		// bpp = 16, ptr = 32-bit
		And3 = 1;
		Shift4 = 4;

		Shift3 = 1;
		
		//if ( FrameBuffer_PixelFormat == 0 || FrameBuffer_PixelFormat == 1 )
		//{
		//	Pixel_SrcMask = 0x1f;
		//	Pixel_SrcBpp = 5;
		//	Pixel_DstShift1 = 3;
		//}
	}
	*/
	
	// important: reading from buffer in 32-bit units, so the texture buffer width should be divided accordingly
	//TexBufWidth >>= Shift1;
	
	
	// get the shade color
	//c1 = GPURegsGp.RGBAQ.R;
	//c2 = GPURegsGp.RGBAQ.G;
	//c3 = GPURegsGp.RGBAQ.B;
	//ca = GPURegsGp.RGBAQ.A;


	if ( !CLUTStoreMode )
	{
		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
	}
	else
	{
		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}

	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	
#ifdef ENABLE_INVERT_ZVALUE
	// invert z for now
	z0 ^= 0xffffffffULL;
	z1 ^= 0xffffffffULL;
	z2 ^= 0xffffffffULL;
#endif

	// get fog
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	f2 = (u64) f [ Coord2 ].F;

	// get S
	fS0 = st [ Coord0 ].fS;
	fS1 = st [ Coord1 ].fS;
	fS2 = st [ Coord2 ].fS;
	
	// get T
	fT0 = st [ Coord0 ].fT;
	fT1 = st [ Coord1 ].fT;
	fT2 = st [ Coord2 ].fT;
	
	// get Q
	fQ0 = rgbaq [ Coord0 ].fQ;
	fQ1 = rgbaq [ Coord1 ].fQ;
	fQ2 = rgbaq [ Coord2 ].fQ;
	
	// prepare S,T,Q values
	fS0 = fS0 * ( TexWidth );
	fS1 = fS1 * ( TexWidth );
	fS2 = fS2 * ( TexWidth );
	
	fT0 = fT0 * ( TexHeight );
	fT1 = fT1 * ( TexHeight );
	fT2 = fT2 * ( TexHeight );
	
	//fQ0 = 1.0f / fQ0;
	//fQ1 = 1.0f / fQ1;
	//fQ2 = 1.0f / fQ2;
	
	
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
		// means using uv coords when FST is set //
		
		uv_temp [ Coord0 ].U = uv [ Coord0 ].U;
		uv_temp [ Coord0 ].V = uv [ Coord0 ].V;
		uv_temp [ Coord1 ].U = uv [ Coord1 ].U;
		uv_temp [ Coord1 ].V = uv [ Coord1 ].V;
		uv_temp [ Coord2 ].U = uv [ Coord2 ].U;
		uv_temp [ Coord2 ].V = uv [ Coord2 ].V;
		
		// if using u,v coords, then the larger point should be minus one
		//if ( u1 > u0 ) u1 -= ( 1 << 4 ); else u0 -= ( 1 << 4 );
		//if ( v1 > v0 ) v1 -= ( 1 << 4 ); else v0 -= ( 1 << 4 );
	}
	else
	{
		// ST coords for triangles not implemented yet //

//#ifdef VERBOSE_TRIANGLE_ST
//		cout << "\nhps2x64: ALERT: GPU: ST coords for triangles not implemented yet!!!\n";
//#endif

#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; STCoords: s0=" << st [ Coord0 ].fS << " t0=" << st [ Coord0 ].fT << " s1=" << st [ Coord1 ].fS << " t1=" << st [ Coord1 ].fT << " s2=" << st [ Coord2 ].fS << " t2=" << st [ Coord2 ].fT;
	debug << " q0=" << rgbaq [ Coord0 ].fQ << " q1=" << rgbaq [ Coord1 ].fQ << " q2=" << rgbaq [ Coord2 ].fQ;
#endif

		// note: texture width/height should probably be minus one
		uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		//uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT ) * ( (float) TexHeight ) * 16.0f );
		
	}

	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
	debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	// coords are in 12.4 fixed point
	//x0 >>= 4;
	//y0 >>= 4;
	//x1 >>= 4;
	//y1 >>= 4;
	//x2 >>= 4;
	//y2 >>= 4;
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleGradientTexture";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " ZBUF=" << ZBUF_X.Value;
		//debug << " MXL=" << TEX1 [ Ctx ].MXL;
		//debug << " LCM=" << TEX1 [ Ctx ].LCM;
		//debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//dudx = ( ( ( ( (s64) ( uv [ Coord0 ].U - uv [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].U - uv [ Coord2 ].U ) ) * t1 ) ) << 12 ) / denominator;
		//dvdx = ( ( ( ( (s64) ( uv [ Coord0 ].V - uv [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].V - uv [ Coord2 ].V ) ) * t1 ) ) << 12 ) / denominator;
		dudx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].U - uv_temp [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].U - uv_temp [ Coord2 ].U ) ) * t1 ) ) << 16 ) / denominator;
		dvdx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].V - uv_temp [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].V - uv_temp [ Coord2 ].V ) ) * t1 ) ) << 16 ) / denominator;
	
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;

		dfdx = ( ( ( ( (s64) ( f0 - f2 ) ) * t0 ) - ( ( (s64) ( f1 - f2 ) ) * t1 ) ) << 28 ) / denominator;

		dSdx = ( ( ( fS0 - fS2 ) * ( t0 / 16.0f ) ) - ( ( fS1 - fS2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dTdx = ( ( ( fT0 - fT2 ) * ( t0 / 16.0f ) ) - ( ( fT1 - fT2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dQdx = ( ( ( fQ0 - fQ2 ) * ( t0 / 16.0f ) ) - ( ( fQ1 - fQ2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		
		drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << 28 ) / denominator;
		dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << 28 ) / denominator;
		dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << 28 ) / denominator;
		dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << 28 ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	//u [ X0Index ] = ( uv [ Coord0 ].U << 12 );
	//v [ X0Index ] = ( uv [ Coord0 ].V << 12 );
	u [ X0Index ] = ( uv_temp [ Coord0 ].U << 12 );
	v [ X0Index ] = ( uv_temp [ Coord0 ].V << 12 );
	
	fS [ X0Index ] = fS0;
	fT [ X0Index ] = fT0;
	fQ [ X0Index ] = fQ0;
	
	z [ X0Index ] = ( ((s64)z0) << 23 );
	fogc [ X0Index ] = ( ((s64)f0) << 24 );
	
	r [ X0Index ] = ( rgbaq [ Coord0 ].R << 24 );
	g [ X0Index ] = ( rgbaq [ Coord0 ].G << 24 );
	b [ X0Index ] = ( rgbaq [ Coord0 ].B << 24 );
	a [ X0Index ] = ( rgbaq [ Coord0 ].A << 24 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		u [ X1Index ] = u [ X0Index ];
		v [ X1Index ] = v [ X0Index ];
		
		fS [ X1Index ] = fS [ X0Index ];
		fT [ X1Index ] = fT [ X0Index ];
		fQ [ X1Index ] = fQ [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		fogc [ X1Index ] = fogc [ X0Index ];
		
		r [ X1Index ] = r [ X0Index ];
		g [ X1Index ] = g [ X0Index ];
		b [ X1Index ] = b [ X0Index ];
		a [ X1Index ] = a [ X0Index ];
		
		//dx_left = (((s64)( x1 - x0 )) << 32 ) / ((s64)( y1 - y0 ));
		//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		//dudy [ X1Index ] = (((s32)( uv [ Coord1 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y1 - y0 ));
		//dvdy [ X1Index ] = (((s32)( uv [ Coord1 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y1 - y0 ));
		dudy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y1 - y0 ));
		dvdy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y1 - y0 ));
		
		dSdy [ X1Index ] = ( fS1 - fS0 ) / ( ( y1 - y0 ) / 16.0f );
		dTdy [ X1Index ] = ( fT1 - fT0 ) / ( ( y1 - y0 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ1 - fQ0 ) / ( ( y1 - y0 ) / 16.0f );
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].R - rgbaq [ Coord0 ].R )) << 28 ) / ((s64)( y1 - y0 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].G - rgbaq [ Coord0 ].G )) << 28 ) / ((s64)( y1 - y0 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].B - rgbaq [ Coord0 ].B )) << 28 ) / ((s64)( y1 - y0 ));
		dady [ X1Index ] = (((s64)( rgbaq [ Coord1 ].A - rgbaq [ Coord0 ].A )) << 28 ) / ((s64)( y1 - y0 ));
		
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		dfdy [ X1Index ] = (((s64)( f1 - f0 )) << 28 ) / ((s64)( y1 - y0 ));
		
		//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
		//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
		dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
		dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));

		dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
		dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
		dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
		
		drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 28 ) / ((s64)( y2 - y0 ));
		dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 28 ) / ((s64)( y2 - y0 ));
		dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 28 ) / ((s64)( y2 - y0 ));
		dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 28 ) / ((s64)( y2 - y0 ));
		
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 28 ) / ((s64)( y2 - y0 ));
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
		//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
		u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
		v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );

		fS [ X1Index ] = fS1;
		fT [ X1Index ] = fT1;
		fQ [ X1Index ] = fQ1;
		
		z [ X1Index ] = ( ((s64)z1) << 23 );
		fogc [ X1Index ] = ( ((s64)f1) << 24 );
		
		r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
		g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
		b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
		a [ X1Index ] = ( rgbaq [ Coord1 ].A << 24 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
			//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
			dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
			dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
			
			dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
			dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
			dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
			
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 28 ) / ((s64)( y2 - y0 ));
			
			// only need to set dr,dg,db for the x0/x2 side here
			drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 28 ) / ((s64)( y2 - y0 ));
			dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 28 ) / ((s64)( y2 - y0 ));
			dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 28 ) / ((s64)( y2 - y0 ));
			dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 28 ) / ((s64)( y2 - y0 ));
		}
	}


	
	
	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}
	
	/*
	if ( FrameBuffer_PixelFormat == 1 )
	{
		// 24-bit frame buffer //
		DestAlpha24 = -0x80000000;
		DestMask24 = 0xffffff;
	}
	else
	{
		// NOT 24-bit frame buffer //
		DestAlpha24 = 0;
		DestMask24 = 0xffffffff;
	}
	*/
	
	//StartY = y0;
	//EndY = y1;

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
	v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
	
	fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
	fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
	fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
	
	r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
	g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
	b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
	a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
	
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
	
	//u [ 1 ] += ( dudy [ 1 ] >> 4 ) * Temp;
	//v [ 1 ] += ( dvdy [ 1 ] >> 4 ) * Temp;
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;

		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		

		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];

			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			iA = a [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );

			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/*
			//DitherLine = & ( DitherArray [ ( Line & 0x3 ) << 2 ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			
#ifdef DEBUG_TEST
	debug << "\r\n";
#endif

//#define INLINE_DEBUG_SHOW_RASTER1
#ifdef INLINE_DEBUG_SHOW_RASTER1
	debug << "\r\n";
	debug << "TOP" << dec << " Line=" << Line << " dSdx=" << dSdx << " dTdx=" << dTdx << " dQdx=" << dQdx;
	debug << " dSdy=" << dSdy[0] << "dTdy=" << dTdy[0] << "dQdy=" << dQdy[0];
	debug << " fS=" << fS[0] << " fT=" << fT[0] << " fQ=" << fQ[0];
#endif


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				TexCoordX = ( iU >> 16 ) /* & TexWidth_Mask */;
				TexCoordY = ( iV >> 16 ) /* & TexHeight_Mask */;

#ifdef ENABLE_3D_TMAPPING_GTRI
				if ( !Fst )
				{
					TexCoordX = (long) (iS / iQ);
					TexCoordY = (long) (iT / iQ);
				}
#endif

//#define INLINE_DEBUG_SHOW_RASTER1
#ifdef INLINE_DEBUG_SHOW_RASTER1
	debug << "\r\n";
	debug << " x_across=" << x_across << " iS=" << iS << " iT=" << iT << " iQ=" << iQ << " tx=" << TexCoordX << " ty=" << TexCoordY;
	//debug << "top y=" << dec << Line << " sx=" << StartX << " ex=" << EndX << hex << " dadx=" << dadx << " dady=" << dady [ 0 ];
	//debug << hex << " S=" << iS << " T=" << iT << " Q=" << iQ << " u=" << iU << " v=" << iV << " f=" << iF << " r=" << iR << " g=" << iG << " b=" << iB << " a=" << iA;
#endif


				TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				TexCoordY &= TexY_And;
				TexCoordY |= TexY_Or;

				TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				TexCoordX &= TexX_And;
				TexCoordX |= TexX_Or;

//if ( Clamp_ModeX == 1 && TexWidth == 64 )
//{
//	debug << "\r\nTexCoordX=" << dec << TexCoordX;
//	debug << " TexCoordY=" << dec << TexCoordY;
//}
				


				// get texel from texture buffer (32-bit frame buffer) //

				//bgr = ptr_texture [ ( TexCoordX >> Shift1 ) + ( TexCoordY * ( TexBufWidth >> Shift1 ) ) ];
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT24
					case 1:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16
					case 2:
						bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16S
					case 0xa:
						bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT8
					case 0x13:
						bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT4
					case 0x14:
						bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						bgr_temp = ( ptr_texture8 [ bgr >> 1 ] >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						break;
						
					// PSMT8H
					case 0x1b:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						break;
						
					// PSMT4HL
					case 0x24:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
						bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16
					case 0x32:
						bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16S
					case 0x3a:
						bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
				}
				
				// look up color value in CLUT if needed //
				//if ( PixelFormat >> 4 )
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					bgr = ptr_clut16 [ bgr_temp ];
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
					}
				}
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// pixel is probably not transparent ?? //
						
						bgr_temp = bgr;
						
						// convert to 32-bit
						bgr = ConvertPixel16To24 ( bgr );
						
						// set the texture alpha for 16 bit pixel
						bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					}
					/*
					else if ( bgr )
					{
						// set the texture alpha for 16 bit pixel
						bgr |= GPURegsGp.TEXA.TA0 << 24;
					}
					*/
				}

				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					bgr &= 0xffffff;
					
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// alpha is TA0
						bgr |= ( GPURegsGp.TEXA.TA0 << 24 );
					}
#ifdef ENABLE_TRANSPARENT24
					else
					{
						// if alpha is not TA0, then it is transparent??
						iU += dudx;
						iV += dvdx;
						
						iS += dSdx;
						iT += dTdx;
						iQ += dQdx;
						
						iR += drdx;
						iG += dgdx;
						iB += dbdx;
						iA += dadx;
						
						iZ += dzdx;
						iF += dfdx;
						continue;
					}
#endif
				}

#ifdef INLINE_DEBUG_SHOW_RASTER
	debug << dec << "; x=" << x_across << " y=" << Line << hex << " bgr=" << bgr << " if=" << iF << " ir=" << iR << " ig=" << iG << " ib=" << iB << " ia=" << iA;
#endif

				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, ( (u32) iR ) >> 24, ( (u32) iG ) >> 24, ( (u32) iB ) >> 24, ( (u32) iA ) >> 24, iF >> 24 );
				
						
				iU += dudx;
				iV += dvdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				
				iZ += dzdx;
				iF += dfdx;
				
				
			}
			
		}
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		//x_left += dx_left;
		//x_right += dx_right;
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		a [ 0 ] += dady [ 0 ];
		
		//u [ 1 ] += dudy [ 1 ];
		//v [ 1 ] += dvdy [ 1 ];
		
		//r [ 1 ] += drdy [ 1 ];
		//g [ 1 ] += dgdy [ 1 ];
		//b [ 1 ] += dbdy [ 1 ];
	}
	
	} // if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
	//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
	u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
	v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );
	
	fS [ X1Index ] = fS1;
	fT [ X1Index ] = fT1;
	fQ [ X1Index ] = fQ1;
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	fogc [ X1Index ] = ( ((s64)f1) << 24 );
	
	r [ X1Index ] = ( rgbaq [ Coord1 ].R << 24 );
	g [ X1Index ] = ( rgbaq [ Coord1 ].G << 24 );
	b [ X1Index ] = ( rgbaq [ Coord1 ].B << 24 );
	a [ X1Index ] = ( rgbaq [ Coord1 ].A << 24 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//dudy [ X1Index ] = (((s64)( uv [ Coord2 ].U - uv [ Coord1 ].U )) << 12 ) / ((s64)( y2 - y1 ));
		//dvdy [ X1Index ] = (((s64)( uv [ Coord2 ].V - uv [ Coord1 ].V )) << 12 ) / ((s64)( y2 - y1 ));
		dudy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].U - uv_temp [ Coord1 ].U )) << 16 ) / ((s64)( y2 - y1 ));
		dvdy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].V - uv_temp [ Coord1 ].V )) << 16 ) / ((s64)( y2 - y1 ));
		
		dSdy [ X1Index ] = ( fS2 - fS1 ) / ( ( y2 - y1 ) / 16.0f );
		dTdy [ X1Index ] = ( fT2 - fT1 ) / ( ( y2 - y1 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ2 - fQ1 ) / ( ( y2 - y1 ) / 16.0f );
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
		dfdy [ X1Index ] = (((s64)( f2 - f1 )) << 28 ) / ((s64)( y2 - y1 ));
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 28 ) / ((s64)( y2 - y1 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 28 ) / ((s64)( y2 - y1 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 28 ) / ((s64)( y2 - y1 ));
		dady [ X1Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord1 ].A )) << 28 ) / ((s64)( y2 - y1 ));
	}
	
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
		debug << dec << " du0=" << (dudy[0]>>16) << " dv0=" << (dvdy[0]>>16) << " du1=" << (dudy[1]>>16) << " dv1=" << (dvdy[1]>>16);
		debug << dec << " dudx=" << (dudx>>16) << " dvdx=" << (dvdx>>16);
		debug << dec << " denominator=" << (denominator>>8) << " t0=" << (t0>>4) << " t1=" << (t1>>4);
#endif
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	
	// the line starts at y1 from here
	//Line = y1;

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	u [ X1Index ] += ( dudy [ X1Index ] >> 4 ) * Temp;
	v [ X1Index ] += ( dvdy [ X1Index ] >> 4 ) * Temp;

	fS [ X1Index ] += ( dSdy [ X1Index ] ) * ( Temp / 16.0f );
	fT [ X1Index ] += ( dTdy [ X1Index ] ) * ( Temp / 16.0f );
	fQ [ X1Index ] += ( dQdy [ X1Index ] ) * ( Temp / 16.0f );
	
	r [ X1Index ] += ( drdy [ X1Index ] >> 4 ) * Temp;
	g [ X1Index ] += ( dgdy [ X1Index ] >> 4 ) * Temp;
	b [ X1Index ] += ( dbdy [ X1Index ] >> 4 ) * Temp;
	a [ X1Index ] += ( dady [ X1Index ] >> 4 ) * Temp;
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	fogc [ X1Index ] += ( dfdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
		v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
		
		fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
		fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
		fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
		
		r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
		g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
		b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
		a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
		
		//u [ 1 ] += ( dudy [ 1 ] >> 4 ) * Temp;
		//v [ 1 ] += ( dvdy [ 1 ] >> 4 ) * Temp;
	}
	

	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		//StartX = _Round( x [ 0 ] ) >> 32;
		//EndX = _Round( x [ 1 ] ) >> 32;
		
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		//if ( EndX < StartX ) EndX = StartX;
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			iA = a [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
		
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			/*
			//ptr = & ( VRAM [ StartX + ( Line << 10 ) ] );
			//DitherLine = & ( DitherArray [ ( Line & 0x3 ) << 2 ] );
			if ( FrameBuffer_PixelFormat < 2 )
			{
				ptr32 = & ( VRAM32 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			else
			{
				ptr16 = & ( VRAM16 [ StartX + ( Line * FrameBufferWidth_Pixels ) ] );
			}
			*/
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			
#ifdef INLINE_DEBUG_SHOW_RASTER
	debug << "\r\n";
	debug << "bottom y=" << dec << Line << " sx=" << StartX << " ex=" << EndX;
	debug << hex << " S=" << iS << " T=" << iT << " Q=" << iQ << " u=" << iU << " v=" << iV << " f=" << iF << " r=" << iR << " g=" << iG << " b=" << iB << " a=" << iA;
#endif

//#define INLINE_DEBUG_SHOW_RASTER1
#ifdef INLINE_DEBUG_SHOW_RASTER1
	debug << "\r\n";
	debug << "BOTTOM" << dec << " Line=" << Line << " dSdx=" << dSdx << " dTdx=" << dTdx << " dQdx=" << dQdx;
	debug << " dSdy=" << dSdy[0] << "dTdy=" << dTdy[0] << "dQdy=" << dQdy[0];
	debug << " fS=" << fS[0] << " fT=" << fT[0] << " fQ=" << fQ[0];
#endif



			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				TexCoordX = ( iU >> 16 ) /* & TexWidth_Mask */;
				TexCoordY = ( iV >> 16 ) /* & TexHeight_Mask */;
				
#ifdef ENABLE_3D_TMAPPING_GTRI
				if ( !Fst )
				{
					TexCoordX = (long) (iS / iQ);
					TexCoordY = (long) (iT / iQ);
				}
#endif
				
//#define INLINE_DEBUG_SHOW_RASTER1
#ifdef INLINE_DEBUG_SHOW_RASTER1
	debug << "\r\n";
	debug << " x_across=" << x_across << " iS=" << iS << " iT=" << iT << " iQ=" << iQ << " tx=" << TexCoordX << " ty=" << TexCoordY;
	//debug << "top y=" << dec << Line << " sx=" << StartX << " ex=" << EndX << hex << " dadx=" << dadx << " dady=" << dady [ 0 ];
	//debug << hex << " S=" << iS << " T=" << iT << " Q=" << iQ << " u=" << iU << " v=" << iV << " f=" << iF << " r=" << iR << " g=" << iG << " b=" << iB << " a=" << iA;
#endif



				TexCoordY = ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY;
				TexCoordY = ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY;
				TexCoordY &= TexY_And;
				TexCoordY |= TexY_Or;

				TexCoordX = ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX;
				TexCoordX = ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX;
				TexCoordX &= TexX_And;
				TexCoordX |= TexX_Or;
				
//if ( Clamp_ModeX == 1 && TexWidth == 64 )
//{
//	debug << "\r\nTexCoordX=" << dec << TexCoordX;
//	debug << " TexCoordY=" << dec << TexCoordY;
//}
				
				// get texel from texture buffer (32-bit frame buffer) //

				//bgr = ptr_texture [ ( TexCoordX >> Shift1 ) + ( TexCoordY * ( TexBufWidth >> Shift1 ) ) ];
				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT24
					case 1:
						bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16
					case 2:
						bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMCT16S
					case 0xa:
						bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT8
					case 0x13:
						bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMT4
					case 0x14:
						bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						bgr_temp = ( ptr_texture8 [ bgr >> 1 ] >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						break;
						
					// PSMT8H
					case 0x1b:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						break;
						
					// PSMT4HL
					case 0x24:
						bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
						bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16
					case 0x32:
						bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
						
					// PSMZ16S
					case 0x3a:
						bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						break;
				}
				
				// look up color value in CLUT if needed //
				//if ( PixelFormat >> 4 )
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					bgr = ptr_clut16 [ bgr_temp ];
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
					}
				}
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// pixel is probably not transparent ?? //
						
						bgr_temp = bgr;
						
						// convert to 32-bit
						bgr = ConvertPixel16To24 ( bgr );
						
						// set the texture alpha for 16 bit pixel
						bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					}
					/*
					else if ( bgr )
					{
						// set the texture alpha for 16 bit pixel
						bgr |= GPURegsGp.TEXA.TA0 << 24;
					}
					*/
				}
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					bgr &= 0xffffff;
					
					if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					{
						// alpha is TA0
						bgr |= ( GPURegsGp.TEXA.TA0 << 24 );
					}
#ifdef ENABLE_TRANSPARENT24
					else
					{
						// if alpha is not TA0, then it is transparent??
						iU += dudx;
						iV += dvdx;
						
						iS += dSdx;
						iT += dTdx;
						iQ += dQdx;
						
						iR += drdx;
						iG += dgdx;
						iB += dbdx;
						iA += dadx;
						
						iZ += dzdx;
						iF += dfdx;
						continue;
					}
#endif
				}
				
#ifdef INLINE_DEBUG_SHOW_RASTER
	debug << dec << "; x=" << x_across << " y=" << Line << hex << " bgr=" << bgr << " if=" << iF << " ir=" << iR << " ig=" << iG << " ib=" << iB << " ia=" << iA;
#endif

				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, ( (u32) iR ) >> 24, ( (u32) iG ) >> 24, ( (u32) iB ) >> 24, ( (u32) iA ) >> 24, iF >> 24 );
				
				
				iU += dudx;
				iV += dvdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				
				iZ += dzdx;
				iF += dfdx;
				
				//iR += drdx;
				//iG += dgdx;
				//iB += dbdx;
				
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		//x_left += dx_left;
		//x_right += dx_right;
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		//u [ 1 ] += dudy [ 1 ];
		//v [ 1 ] += dvdy [ 1 ];
		
		//r [ 1 ] += drdy [ 1 ];
		//g [ 1 ] += dgdy [ 1 ];
		//b [ 1 ] += dbdy [ 1 ];
	}
	
	} // if ( EndY >= StartY )

}


#ifdef _ENABLE_SSE41_TRIANGLE_TEXTURE_GRADIENT

void GPU::DrawTriangle_GradientTexture32_DSx4 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 4;

	
	u16 *CLUT_LUT;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;
	
	u32 *ptr_texture32;
	u16 *ptr_texture16;
	u8 *ptr_texture8;
	
	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	//s64 Error_Left;
	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];

	float fS0, fS1, fS2, fT0, fT1, fT2, fQ0, fQ1, fQ2;
	float fS [ 2 ], fT [ 2 ], fQ [ 2 ], dSdy [ 2 ], dTdy [ 2 ], dQdy [ 2 ];
	float iS, iT, iQ, dSdx, dTdx, dQdx;
	
	s64 iU, iV;
	
	s64 iR, iG, iB;
	
	s64 u [ 2 ], v [ 2 ];
	s64 dudy [ 2 ], dvdy [ 2 ];
	
	s64 r [ 2 ], g [ 2 ], b [ 2 ];
	s64 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	
	s64 dudx, dvdx;
	s64 drdx, dgdx, dbdx;
	
	s64 a [ 2 ];
	s64 dady [ 2 ];
	s64 dadx;
	s64 iA;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;

	// fog coords ?
	s64 f0, f1, f2;
	s64 fogc [ 2 ], dfdy [ 2 ];
	s64 dfdx;
	s64 iF;
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel, PixelMask = 0, SetPixelMask = 0;


	s32 TexCoordX, TexCoordY;
	u32 TextureOffset;
	
	
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	
	
	__m128i vEnable;
	__m128i vIdentity;
	vIdentity = _mm_set_epi32 ( 3, 2, 1, 0 );

	__m128 vIdentityf;
	vIdentityf = _mm_set_ps ( 3.0f, 2.0f, 1.0f, 0.0f );
	
	__m128i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m128i vXInc;
	__m128i vYInc;
	vXInc = _mm_set1_epi32 ( c_iVectorSize );
	vYInc = _mm_set1_epi32 ( 1 );
	
	__m128i vZInc;
	__m128i vZIdentity0, vZIdentity1;
	__m128i vZacross0, vZacross1;
	
	__m128i vFInc;
	__m128i vFIdentity;
	__m128i vFacross;
	
	__m128i vUInc, vVInc;
	__m128i vUIdentity, vVIdentity;
	__m128i vUacross, vVacross;
	
	__m128 vSInc, vTInc, vQInc;
	__m128 vSIdentity, vTIdentity, vQIdentity;
	__m128 vSacross, vTacross, vQacross;
	
	__m128i vTexOffset32, vTexBufWidth;
	__m128i vTexCoordY, vTexY_Min, vTexY_Max, vTexY_And, vTexY_Or;
	__m128i vTexCoordX, vTexX_Min, vTexX_Max, vTexX_And, vTexX_Or;
	//__m128i vRed, vGreen, vBlue, vAlpha;
	__m128i vBgr_temp, vBgr2, vca, vc1, vc2, vc3;

	__m128i vRInc, vGInc, vBInc, vAInc;
	__m128i vRIdentity, vGIdentity, vBIdentity, vAIdentity;
	__m128i vRacross, vGacross, vBacross, vAacross;
	__m128i vRed, vGreen, vBlue, vAlpha;

	
	
	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;
	
	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;


	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	CLUTOffset = TEX0 [ Ctx ].CSA;


	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	ptr_texture32 = (u32*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture8 = (u8*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	
	// check if the texture pixel format is bgr24 or bgr32
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	
	
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			break;
			
		// PSMCT24
		case 1:
			break;
			
		// PSMCT16
		case 2:
			break;
			
		// PSMCT16S
		case 0xa:
			break;
			
		// PSMT8
		case 0x13:
			break;
			
		// PSMT4
		case 0x14:
			break;
			
		// PSMT8H
		case 0x1b:
			break;
			
		// PSMT4HL
		case 0x24:
			break;
			
		// PSMT4HH
		case 0x2c:
			break;
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
			break;
			
		// PSMZ16
		case 0x32:
			break;
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	


	if ( !CLUTStoreMode )
	{
		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
	}
	else
	{
		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}

	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	

	// get fog
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	f2 = (u64) f [ Coord2 ].F;

	// get S
	fS0 = st [ Coord0 ].fS;
	fS1 = st [ Coord1 ].fS;
	fS2 = st [ Coord2 ].fS;
	
	// get T
	fT0 = st [ Coord0 ].fT;
	fT1 = st [ Coord1 ].fT;
	fT2 = st [ Coord2 ].fT;
	
	// get Q
	fQ0 = rgbaq [ Coord0 ].fQ;
	fQ1 = rgbaq [ Coord1 ].fQ;
	fQ2 = rgbaq [ Coord2 ].fQ;
	
	// prepare S,T,Q values
	fS0 = fS0 * ( TexWidth );
	fS1 = fS1 * ( TexWidth );
	fS2 = fS2 * ( TexWidth );
	
	fT0 = fT0 * ( TexHeight );
	fT1 = fT1 * ( TexHeight );
	fT2 = fT2 * ( TexHeight );
	
	
	
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
		// means using uv coords when FST is set //
		
		uv_temp [ Coord0 ].U = uv [ Coord0 ].U;
		uv_temp [ Coord0 ].V = uv [ Coord0 ].V;
		uv_temp [ Coord1 ].U = uv [ Coord1 ].U;
		uv_temp [ Coord1 ].V = uv [ Coord1 ].V;
		uv_temp [ Coord2 ].U = uv [ Coord2 ].U;
		uv_temp [ Coord2 ].V = uv [ Coord2 ].V;
		
	}
	else
	{
		// ST coords for triangles not implemented yet //


#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; STCoords: s0=" << st [ Coord0 ].fS << " t0=" << st [ Coord0 ].fT << " s1=" << st [ Coord1 ].fS << " t1=" << st [ Coord1 ].fT << " s2=" << st [ Coord2 ].fS << " t2=" << st [ Coord2 ].fT;
	debug << " q0=" << rgbaq [ Coord0 ].fQ << " q1=" << rgbaq [ Coord1 ].fQ << " q2=" << rgbaq [ Coord2 ].fQ;
#endif

		// note: texture width/height should probably be minus one
		uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		//uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT ) * ( (float) TexHeight ) * 16.0f );
		
	}

	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
	debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleGradientTexture";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " ZBUF=" << ZBUF_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//dudx = ( ( ( ( (s64) ( uv [ Coord0 ].U - uv [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].U - uv [ Coord2 ].U ) ) * t1 ) ) << 12 ) / denominator;
		//dvdx = ( ( ( ( (s64) ( uv [ Coord0 ].V - uv [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].V - uv [ Coord2 ].V ) ) * t1 ) ) << 12 ) / denominator;
		dudx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].U - uv_temp [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].U - uv_temp [ Coord2 ].U ) ) * t1 ) ) << 16 ) / denominator;
		dvdx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].V - uv_temp [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].V - uv_temp [ Coord2 ].V ) ) * t1 ) ) << 16 ) / denominator;
	
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;

		dfdx = ( ( ( ( (s64) ( f0 - f2 ) ) * t0 ) - ( ( (s64) ( f1 - f2 ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;

		dSdx = ( ( ( fS0 - fS2 ) * ( t0 / 16.0f ) ) - ( ( fS1 - fS2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dTdx = ( ( ( fT0 - fT2 ) * ( t0 / 16.0f ) ) - ( ( fT1 - fT2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dQdx = ( ( ( fQ0 - fQ2 ) * ( t0 / 16.0f ) ) - ( ( fQ1 - fQ2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		
		drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	//u [ X0Index ] = ( uv [ Coord0 ].U << 12 );
	//v [ X0Index ] = ( uv [ Coord0 ].V << 12 );
	u [ X0Index ] = ( uv_temp [ Coord0 ].U << 12 );
	v [ X0Index ] = ( uv_temp [ Coord0 ].V << 12 );
	
	fS [ X0Index ] = fS0;
	fT [ X0Index ] = fT0;
	fQ [ X0Index ] = fQ0;
	
	z [ X0Index ] = ( ((s64)z0) << 23 );
	fogc [ X0Index ] = ( ((s64)f0) << 16 );
	
	r [ X0Index ] = ( rgbaq [ Coord0 ].R << 16 );
	g [ X0Index ] = ( rgbaq [ Coord0 ].G << 16 );
	b [ X0Index ] = ( rgbaq [ Coord0 ].B << 16 );
	a [ X0Index ] = ( rgbaq [ Coord0 ].A << 16 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		u [ X1Index ] = u [ X0Index ];
		v [ X1Index ] = v [ X0Index ];
		
		fS [ X1Index ] = fS [ X0Index ];
		fT [ X1Index ] = fT [ X0Index ];
		fQ [ X1Index ] = fQ [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		fogc [ X1Index ] = fogc [ X0Index ];
		
		r [ X1Index ] = r [ X0Index ];
		g [ X1Index ] = g [ X0Index ];
		b [ X1Index ] = b [ X0Index ];
		a [ X1Index ] = a [ X0Index ];
		
		//dx_left = (((s64)( x1 - x0 )) << 32 ) / ((s64)( y1 - y0 ));
		//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		//dudy [ X1Index ] = (((s32)( uv [ Coord1 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y1 - y0 ));
		//dvdy [ X1Index ] = (((s32)( uv [ Coord1 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y1 - y0 ));
		dudy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y1 - y0 ));
		dvdy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y1 - y0 ));
		
		dSdy [ X1Index ] = ( fS1 - fS0 ) / ( ( y1 - y0 ) / 16.0f );
		dTdy [ X1Index ] = ( fT1 - fT0 ) / ( ( y1 - y0 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ1 - fQ0 ) / ( ( y1 - y0 ) / 16.0f );
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].R - rgbaq [ Coord0 ].R )) << 20 ) / ((s64)( y1 - y0 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].G - rgbaq [ Coord0 ].G )) << 20 ) / ((s64)( y1 - y0 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].B - rgbaq [ Coord0 ].B )) << 20 ) / ((s64)( y1 - y0 ));
		dady [ X1Index ] = (((s64)( rgbaq [ Coord1 ].A - rgbaq [ Coord0 ].A )) << 20 ) / ((s64)( y1 - y0 ));
		
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		dfdy [ X1Index ] = (((s64)( f1 - f0 )) << 20 ) / ((s64)( y1 - y0 ));
		
		//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
		//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
		dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
		dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));

		dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
		dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
		dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
		
		drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 20 ) / ((s64)( y2 - y0 ));
		dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 20 ) / ((s64)( y2 - y0 ));
		dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 20 ) / ((s64)( y2 - y0 ));
		dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 20 ) / ((s64)( y2 - y0 ));
		
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
		//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
		u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
		v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );

		fS [ X1Index ] = fS1;
		fT [ X1Index ] = fT1;
		fQ [ X1Index ] = fQ1;
		
		z [ X1Index ] = ( ((s64)z1) << 23 );
		fogc [ X1Index ] = ( ((s64)f1) << 16 );
		
		r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
		g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
		b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );
		a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
			//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
			dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
			dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
			
			dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
			dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
			dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
			
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
			
			// only need to set dr,dg,db for the x0/x2 side here
			drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 20 ) / ((s64)( y2 - y0 ));
			dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 20 ) / ((s64)( y2 - y0 ));
			dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 20 ) / ((s64)( y2 - y0 ));
			dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 20 ) / ((s64)( y2 - y0 ));
		}
	}
	
	
	
	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}
	

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
	v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
	
	fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
	fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
	fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
	
	r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
	g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
	b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
	a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
	
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	
	
	//vAlphaSelect [ 0 ] = _mm_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm_set1_epi32 ( AlphaSelect [ 3 ] );

	
	vZIdentity0 = _mm_set_epi64x ( dzdx * 2, 0 );
	vZIdentity1 = _mm_set_epi64x ( dzdx * 3, dzdx * 1 );
	vZInc = _mm_set1_epi64x ( dzdx * c_iVectorSize );

	vTexBufWidth = _mm_set1_epi32 ( TexBufWidth );
	
	vTexY_Min = _mm_set1_epi32 ( TexY_Min );
	vTexY_Max = _mm_set1_epi32 ( TexY_Max );
	vTexY_And = _mm_set1_epi32 ( TexY_And );
	vTexY_Or = _mm_set1_epi32 ( TexY_Or );
	
	vTexX_Min = _mm_set1_epi32 ( TexX_Min );
	vTexX_Max = _mm_set1_epi32 ( TexX_Max );
	vTexX_And = _mm_set1_epi32 ( TexX_And );
	vTexX_Or = _mm_set1_epi32 ( TexX_Or );
	
	//vc1 = _mm_set1_epi32 ( c1 );
	//vc2 = _mm_set1_epi32 ( c2 );
	//vc3 = _mm_set1_epi32 ( c3 );
	//vca = _mm_set1_epi32 ( ca );

	vUInc = _mm_set1_epi32 ( dudx * c_iVectorSize );
	vVInc = _mm_set1_epi32 ( dvdx * c_iVectorSize );
	
	vFInc = _mm_set1_epi32 ( dfdx * c_iVectorSize );
	
	vRInc = _mm_set1_epi32 ( drdx * c_iVectorSize );
	vGInc = _mm_set1_epi32 ( dgdx * c_iVectorSize );
	vBInc = _mm_set1_epi32 ( dbdx * c_iVectorSize );
	vAInc = _mm_set1_epi32 ( dadx * c_iVectorSize );
	
	vSInc = _mm_set_ps1 ( dSdx * c_iVectorSize );
	vTInc = _mm_set_ps1 ( dTdx * c_iVectorSize );
	vQInc = _mm_set_ps1 ( dQdx * c_iVectorSize );
	
	vUIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dudx ), vIdentity );
	vVIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dvdx ), vIdentity );
	
	vFIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dfdx ), vIdentity );
	
	vRIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( drdx ), vIdentity );
	vGIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dgdx ), vIdentity );
	vBIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dbdx ), vIdentity );
	vAIdentity = _mm_mullo_epi32 ( _mm_set1_epi32 ( dadx ), vIdentity );

	vSIdentity = _mm_mul_ps ( _mm_set_ps1 ( dSdx ), vIdentityf );
	vTIdentity = _mm_mul_ps ( _mm_set_ps1 ( dTdx ), vIdentityf );
	vQIdentity = _mm_mul_ps ( _mm_set_ps1 ( dQdx ), vIdentityf );
	
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{

		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		

		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];

			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			iA = a [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );

			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vYacross = _mm_set1_epi32 ( Line );
			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm_add_epi32 ( _mm_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm_add_epi32 ( _mm_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm_add_epi32 ( _mm_set1_epi32 ( iF ), vFIdentity );
			
			vRacross = _mm_add_epi32 ( _mm_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm_add_epi32 ( _mm_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm_add_epi32 ( _mm_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm_add_epi32 ( _mm_set1_epi32 ( iA ), vAIdentity );
			
			vSacross = _mm_add_ps ( _mm_set_ps1 ( iS ), vSIdentity );
			vTacross = _mm_add_ps ( _mm_set_ps1 ( iT ), vTIdentity );
			vQacross = _mm_add_ps ( _mm_set_ps1 ( iQ ), vQIdentity );


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm_cvttps_epi32 ( _mm_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm_cvttps_epi32 ( _mm_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm_and_si128 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm_or_si128 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm_and_si128 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm_or_si128 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 24 ), 24 );
#else
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 28 ), 28 );
#else
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> _mm_extract_epi32 ( vBgr, 3 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> _mm_extract_epi32 ( vBgr, 2 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> _mm_extract_epi32 ( vBgr, 1 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> _mm_extract_epi32 ( vBgr, 0 ) ) & 0xf );
#endif
						
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 24 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 );
#endif
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 4 ), 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 ) & 0xf );
#endif
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 28 );
#endif
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
#ifdef ENABLE_AVX128
					vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
					//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_clut16, vBgr_temp, 2 );
#else
					//bgr = ptr_clut16 [ bgr_temp ];
					vBgr = _mm_set_epi32 (
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) ] );
#endif
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
#ifdef ENABLE_AVX128
						vBgr2 = _mm_mask_i32gather_epi32 ( vBgr2, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						//vBgr2 = _mm_i32gather_epi32 ( (int const*) ( ptr_clut16 + 256 ), vBgr_temp, 2 );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#else
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
						vBgr2 = _mm_set_epi32 (
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) + 256 ] );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#endif
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 + 3 ) ), _mm_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm_or_si128 ( vBgr, _mm_blendv_epi8 ( vTA0, vTA1, _mm_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm_or_si128 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				vc1 = _mm_srli_epi32 ( vRacross, 16 );
				vc2 = _mm_srli_epi32 ( vGacross, 16 );
				vc3 = _mm_srli_epi32 ( vBacross, 16 );
				vca = _mm_srli_epi32 ( vAacross, 16 );
				
				// texture function ??
				vBgr = TextureFunc32x4 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vBgr = FogFunc32x4 ( vBgr, _mm_srli_epi32 ( vFacross, 16 ) );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;

				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );

				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
				/*
				iU += dudx;
				iV += dvdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				
				iZ += dzdx;
				iF += dfdx;
				*/
				
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm_add_epi32 ( vUacross, vUInc );
				vVacross = _mm_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm_add_epi32 ( vFacross, vFInc );
				
				vRacross = _mm_add_epi32 ( vRacross, vRInc );
				vGacross = _mm_add_epi32 ( vGacross, vGInc );
				vBacross = _mm_add_epi32 ( vBacross, vBInc );
				vAacross = _mm_add_epi32 ( vAacross, vAInc );
				
				vSacross = _mm_add_ps ( vSacross, vSInc );
				vTacross = _mm_add_ps ( vTacross, vTInc );
				vQacross = _mm_add_ps ( vQacross, vQInc );
				
			}
			
		}
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		//x_left += dx_left;
		//x_right += dx_right;
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		a [ 0 ] += dady [ 0 ];
		
	}
	
	} // if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
	//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
	u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
	v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );
	
	fS [ X1Index ] = fS1;
	fT [ X1Index ] = fT1;
	fQ [ X1Index ] = fQ1;
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	fogc [ X1Index ] = ( ((s64)f1) << 16 );
	
	r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
	g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
	b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );
	a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//dudy [ X1Index ] = (((s64)( uv [ Coord2 ].U - uv [ Coord1 ].U )) << 12 ) / ((s64)( y2 - y1 ));
		//dvdy [ X1Index ] = (((s64)( uv [ Coord2 ].V - uv [ Coord1 ].V )) << 12 ) / ((s64)( y2 - y1 ));
		dudy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].U - uv_temp [ Coord1 ].U )) << 16 ) / ((s64)( y2 - y1 ));
		dvdy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].V - uv_temp [ Coord1 ].V )) << 16 ) / ((s64)( y2 - y1 ));
		
		dSdy [ X1Index ] = ( fS2 - fS1 ) / ( ( y2 - y1 ) / 16.0f );
		dTdy [ X1Index ] = ( fT2 - fT1 ) / ( ( y2 - y1 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ2 - fQ1 ) / ( ( y2 - y1 ) / 16.0f );
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
		dfdy [ X1Index ] = (((s64)( f2 - f1 )) << 20 ) / ((s64)( y2 - y1 ));
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 20 ) / ((s64)( y2 - y1 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 20 ) / ((s64)( y2 - y1 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 20 ) / ((s64)( y2 - y1 ));
		dady [ X1Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord1 ].A )) << 20 ) / ((s64)( y2 - y1 ));
	}
	
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	
	// the line starts at y1 from here
	//Line = y1;

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	u [ X1Index ] += ( dudy [ X1Index ] >> 4 ) * Temp;
	v [ X1Index ] += ( dvdy [ X1Index ] >> 4 ) * Temp;

	fS [ X1Index ] += ( dSdy [ X1Index ] ) * ( Temp / 16.0f );
	fT [ X1Index ] += ( dTdy [ X1Index ] ) * ( Temp / 16.0f );
	fQ [ X1Index ] += ( dQdy [ X1Index ] ) * ( Temp / 16.0f );
	
	r [ X1Index ] += ( drdy [ X1Index ] >> 4 ) * Temp;
	g [ X1Index ] += ( dgdy [ X1Index ] >> 4 ) * Temp;
	b [ X1Index ] += ( dbdy [ X1Index ] >> 4 ) * Temp;
	a [ X1Index ] += ( dady [ X1Index ] >> 4 ) * Temp;
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	fogc [ X1Index ] += ( dfdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
		v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
		
		fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
		fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
		fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
		
		r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
		g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
		b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
		a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
		
	}
	

	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			iA = a [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
		
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vYacross = _mm_set1_epi32 ( Line );
			vXacross = _mm_add_epi32 ( _mm_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm_add_epi64 ( _mm_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm_add_epi32 ( _mm_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm_add_epi32 ( _mm_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm_add_epi32 ( _mm_set1_epi32 ( iF ), vFIdentity );
			
			vRacross = _mm_add_epi32 ( _mm_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm_add_epi32 ( _mm_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm_add_epi32 ( _mm_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm_add_epi32 ( _mm_set1_epi32 ( iA ), vAIdentity );
			
			vSacross = _mm_add_ps ( _mm_set_ps1 ( iS ), vSIdentity );
			vTacross = _mm_add_ps ( _mm_set_ps1 ( iT ), vTIdentity );
			vQacross = _mm_add_ps ( _mm_set_ps1 ( iQ ), vQIdentity );

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				vEnable = _mm_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm_cvttps_epi32 ( _mm_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm_cvttps_epi32 ( _mm_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm_and_si128 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm_or_si128 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm_and_si128 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm_or_si128 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrPix16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 24 ), 24 );
#else
						vTexOffset32 = CvtAddrPix8x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 1 );
						vBgr_temp = _mm_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 28 ), 28 );
#else
						vTexOffset32 = CvtAddrPix4x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm_srli_epi32 ( vTexOffset32, 1 );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> _mm_extract_epi32 ( vBgr, 3 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> _mm_extract_epi32 ( vBgr, 2 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> _mm_extract_epi32 ( vBgr, 1 ) ) & 0xf,
								( ptr_texture8 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> _mm_extract_epi32 ( vBgr, 0 ) ) & 0xf );
#endif
						
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 24 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 );
#endif
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr_temp, 4 ), 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 24 ) & 0xf,
								( ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 24 ) & 0xf );
#endif
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr_temp = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
						vBgr_temp = _mm_srli_epi32 ( vBgr_temp, 28 );
#else
						vTexOffset32 = CvtAddrPix32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr_temp = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ] >> 28,
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] >> 28 );
#endif
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 4 );
#else
						vTexOffset32 = CvtAddrZBuf32x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture32 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16x4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
#ifdef ENABLE_AVX128
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_texture32, vTexOffset32, 2 );
#else
						vTexOffset32 = CvtAddrZBuf16Sx4 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm_set_epi32 (
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 3 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 2 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 1 ) ],
								ptr_texture16 [ _mm_extract_epi32 ( vTexOffset32, 0 ) ] );
#endif
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
#ifdef ENABLE_AVX128
					vBgr = _mm_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
					//vBgr = _mm_i32gather_epi32 ( (int const*) ptr_clut16, vBgr_temp, 2 );
#else
					//bgr = ptr_clut16 [ bgr_temp ];
					vBgr = _mm_set_epi32 (
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) ],
							ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) ] );
#endif
					
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
#ifdef ENABLE_AVX128
						vBgr2 = _mm_mask_i32gather_epi32 ( vBgr2, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						//vBgr2 = _mm_i32gather_epi32 ( (int const*) ( ptr_clut16 + 256 ), vBgr_temp, 2 );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#else
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
						vBgr2 = _mm_set_epi32 (
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 3 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 2 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 1 ) + 256 ],
								ptr_clut16 [ _mm_extract_epi32 ( vBgr_temp, 0 ) + 256 ] );
						vBgr = _mm_or_si128 ( vBgr, _mm_slli_epi32 ( vBgr2, 16 ) );
#endif
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm_or_si128 ( _mm_or_si128 ( vRed, _mm_slli_epi32 ( vGreen, 8 + 3 ) ), _mm_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm_or_si128 ( vBgr, _mm_blendv_epi8 ( vTA0, vTA1, _mm_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_setzero_si128 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm_srli_epi32 ( _mm_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm_or_si128 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm_andnot_si128 ( _mm_cmpeq_epi32 ( vBgr_temp, _mm_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				vc1 = _mm_srli_epi32 ( vRacross, 16 );
				vc2 = _mm_srli_epi32 ( vGacross, 16 );
				vc3 = _mm_srli_epi32 ( vBacross, 16 );
				vca = _mm_srli_epi32 ( vAacross, 16 );
				
				// texture function ??
				vBgr = TextureFunc32x4 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vBgr = FogFunc32x4 ( vBgr, _mm_srli_epi32 ( vFacross, 16 ) );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;

				
				vZacross = _mm_blend_epi16 ( _mm_srli_epi64 ( vZacross0, 23 ), _mm_slli_epi64 ( vZacross1, 9 ), 0xcc );

				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox4 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
				/*
				iU += dudx;
				iV += dvdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				
				iZ += dzdx;
				iF += dfdx;
				*/
				
				vXacross = _mm_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm_add_epi32 ( vUacross, vUInc );
				vVacross = _mm_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm_add_epi32 ( vFacross, vFInc );
				
				vRacross = _mm_add_epi32 ( vRacross, vRInc );
				vGacross = _mm_add_epi32 ( vGacross, vGInc );
				vBacross = _mm_add_epi32 ( vBacross, vBInc );
				vAacross = _mm_add_epi32 ( vAacross, vAInc );
				
				vSacross = _mm_add_ps ( vSacross, vSInc );
				vTacross = _mm_add_ps ( vTacross, vTInc );
				vQacross = _mm_add_ps ( vQacross, vQInc );
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
	}
	
	} // if ( EndY >= StartY )

}

#endif

#ifdef _ENABLE_AVX2_TRIANGLE_TEXTURE_GRADIENT

void GPU::DrawTriangle_GradientTexture32_DSx8 ( u32 Coord0, u32 Coord1, u32 Coord2 )
{
	// before sse2, was sending 1 pixels at a time
	static const int c_iVectorSize = 8;

	
	u16 *CLUT_LUT;
	
	u32 color_add;
	
	u32 *ptr_texture;
	u32 *ptr_clut;
	u16 *ptr_clut16;
	u32 clut_width, clut_x, clut_y;
	
	u32 *ptr_texture32;
	u16 *ptr_texture16;
	u8 *ptr_texture8;
	
	// not used
	//u32 clut_xoffset, clut_yoffset;
	
	//u16 *ptr;
	
	s64 Temp;
	s64 LeftMostX, RightMostX, TopMostY, BottomMostY;
	
	s32 StartX, EndX, StartY, EndY;

	//s64 Error_Left;
	
	s64* DitherArray;
	s64* DitherLine;
	s64 DitherValue;
	
	
	s32 x0, y0, x1, y1, x2, y2;
	s64 x [ 2 ], dxdy [ 2 ];

	float fS0, fS1, fS2, fT0, fT1, fT2, fQ0, fQ1, fQ2;
	float fS [ 2 ], fT [ 2 ], fQ [ 2 ], dSdy [ 2 ], dTdy [ 2 ], dQdy [ 2 ];
	float iS, iT, iQ, dSdx, dTdx, dQdx;
	
	s64 iU, iV;
	
	s64 iR, iG, iB;
	
	s64 u [ 2 ], v [ 2 ];
	s64 dudy [ 2 ], dvdy [ 2 ];
	
	s64 r [ 2 ], g [ 2 ], b [ 2 ];
	s64 drdy [ 2 ], dgdy [ 2 ], dbdy [ 2 ];
	
	s64 dudx, dvdx;
	s64 drdx, dgdx, dbdx;
	
	s64 a [ 2 ];
	s64 dady [ 2 ];
	s64 dadx;
	s64 iA;
	
	u32 bgr, bgr_temp;
	u32 bgr16;
	
	// z coords ?
	s64 z0, z1, z2;
	s64 z [ 2 ], dzdy [ 2 ];
	s64 dzdx;
	s64 iZ;

	// fog coords ?
	s64 f0, f1, f2;
	s64 fogc [ 2 ], dfdy [ 2 ];
	s64 dfdx;
	s64 iF;
	
	
	u32 NumberOfPixelsDrawn;
	s32 Line, x_across;
	
	
	//u32 Coord0 = 0, Coord1 = 1, Coord2 = 2;
	u32 X1Index = 0, X0Index = 1;
	
	s64 Red, Green, Blue;
	u32 DestPixel, PixelMask = 0, SetPixelMask = 0;


	s32 TexCoordX, TexCoordY;
	u32 TextureOffset;
	
	
	
	u32 PixelFormat;
	u32 CLUTBufBase32, CLUTPixelFormat, CLUTStoreMode, CLUTOffset;
	u32 TexBufWidth, TexWidth, TexHeight, TexWidth_Mask, TexHeight_Mask;
	
	s32 TexX_Min, TexX_Max, TexY_Min, TexY_Max;
	u32 TexX_And, TexX_Or, TexY_And, TexY_Or;
	
	
	__m256i vEnable;
	__m256i vIdentity;
	vIdentity = _mm256_set_epi32 ( 7, 6, 5, 4, 3, 2, 1, 0 );

	__m256 vIdentityf;
	vIdentityf = _mm256_set_ps ( 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f );
	
	__m256i vStartX, vXacross, vXmax, vYacross, vZacross, vBgr;
	__m256i vXInc;
	__m256i vYInc;
	vXInc = _mm256_set1_epi32 ( c_iVectorSize );
	vYInc = _mm256_set1_epi32 ( 1 );
	
	__m256i vZInc;
	__m256i vZIdentity0, vZIdentity1;
	__m256i vZacross0, vZacross1;
	
	__m256i vFInc;
	__m256i vFIdentity;
	__m256i vFacross;
	
	__m256i vUInc, vVInc;
	__m256i vUIdentity, vVIdentity;
	__m256i vUacross, vVacross;
	
	__m256 vSInc, vTInc, vQInc;
	__m256 vSIdentity, vTIdentity, vQIdentity;
	__m256 vSacross, vTacross, vQacross;
	
	__m256i vTexOffset32, vTexBufWidth;
	__m256i vTexCoordY, vTexY_Min, vTexY_Max, vTexY_And, vTexY_Or;
	__m256i vTexCoordX, vTexX_Min, vTexX_Max, vTexX_And, vTexX_Or;
	//__m256i vRed, vGreen, vBlue, vAlpha;
	__m256i vBgr_temp, vBgr2, vca, vc1, vc2, vc3;

	__m256i vRInc, vGInc, vBInc, vAInc;
	__m256i vRIdentity, vGIdentity, vBIdentity, vAIdentity;
	__m256i vRacross, vGacross, vBacross, vAacross;
	__m256i vRed, vGreen, vBlue, vAlpha;
	__m256i vF;
	
	
	// set fixed alpha values
	AlphaSelect [ 2 ] = ALPHA_X.FIX << 24;
	AlphaSelect [ 3 ] = 0;
	
	// color lookup table width is stored in TEXCLUT register (in pixels/64)
	// get clut width and x in pixels
	clut_width = GPURegsGp.TEXCLUT.CBW << 6;
	clut_x = GPURegsGp.TEXCLUT.COU << 6;
	
	// get clut y in units of pixels
	clut_y = GPURegsGp.TEXCLUT.COV;
	
	
	//TEX0_t *TEX0 = &GPURegsGp.TEX0_1;
	TEX0_t *TEX0 = TEXX;
	
	TEX1_t *TEX1 = &GPURegsGp.TEX1_1;
	
	
	// get the color for the triangle
	//bgr = rgbaq [ 0 ].Value & 0xffffffffULL;
	
	// initialize number of pixels drawn
	NumberOfPixelsDrawn = 0;


	TexBufWidth = TEX0 [ Ctx ].TBW0 << 6;
	TexWidth = 1 << TEX0 [ Ctx ].TW;
	TexHeight = 1 << TEX0 [ Ctx ].TH;
	TexWidth_Mask = TexWidth - 1;
	TexHeight_Mask = TexHeight - 1;
	
	
	
	
	/////////////////////////////////////////////////////////
	// Get offset into texture page
	TexBufStartOffset32 = TEX0 [ Ctx ].TBP0 << 6;
	
	// get the pixel format (16 bit, 32 bit, etc)
	PixelFormat = TEX0 [ Ctx ].PSM;
	CLUTPixelFormat = TEX0 [ Ctx ].CPSM;
	
	// get base pointer to color lookup table (32-bit word address divided by 64)
	CLUTBufBase32 = TEX0 [ Ctx ].CBP << 6;
	
	// storage mode ??
	CLUTStoreMode = TEX0 [ Ctx ].CSM;
	
	// clut offset ??
	CLUTOffset = TEX0 [ Ctx ].CSA;


	
	//////////////////////////////////////////////////////
	// Get offset into color lookup table
	
	//clut_xoffset = 0;
	//ptr_clut = & ( VRAM [ clut_y << 10 ] );
	ptr_clut = & ( RAM32 [ CLUTBufBase32 ] );
	//ptr_texture = & ( VRAM [ ( tpage_tx << 6 ) + ( ( tpage_ty << 8 ) << 10 ) ] );
	ptr_texture = & ( RAM32 [ TexBufStartOffset32 ] );
	
	ptr_texture32 = (u32*) ptr_texture;
	ptr_texture16 = (u16*) ptr_texture;
	ptr_texture8 = (u8*) ptr_texture;
	
	// x offset for CLUT in units of pixels
	//clut_xoffset = CLUTOffset << 4;
	
	
	
	// check if the texture pixel format is bgr24 or bgr32
	if ( PixelFormat == 1 )
	{
		// rgb24 //
		
		// TEXA.TA0 is the alpha of rgb24 value
		RGB24_Alpha = GPURegsGp.TEXA.TA0 << 24;
		
		// 24-bit pixel only
		Pixel24_Mask = 0xffffff;
		
		// r,g,b=0 is transparent for rgb24 when TEXA.AEM is one
		RGB24_TAlpha = ( GPURegsGp.TEXA.AEM ^ 1 ) << 31;
	}
	else
	{
		// NOT rgb24 //
		RGB24_Alpha = 0;
		Pixel24_Mask = 0xffffffff;
		RGB24_TAlpha = 0;
	}
	
	
	

	switch ( PixelFormat )
	{
		// PSMCT32
		case 0:
			break;
			
		// PSMCT24
		case 1:
			break;
			
		// PSMCT16
		case 2:
			break;
			
		// PSMCT16S
		case 0xa:
			break;
			
		// PSMT8
		case 0x13:
			break;
			
		// PSMT4
		case 0x14:
			break;
			
		// PSMT8H
		case 0x1b:
			break;
			
		// PSMT4HL
		case 0x24:
			break;
			
		// PSMT4HH
		case 0x2c:
			break;
			
		// Z-buffer formats
		
		// PSMZ32
		case 0x30:
		
		// PSMZ24
		case 0x31:
			break;
			
		// PSMZ16
		case 0x32:
			break;
			
		// PSMZ16S
		case 0x3a:
			break;
			
		// UNKNOWN PIXEL FORMAT
		default:
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
#endif
#ifdef INLINE_DEBUG_UNKNOWN_PIXEL
			debug << "\r\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Unknown Pixel Format: " << hex << PixelFormat << " Cycle#" << dec << *_DebugCycleCount << "\n";
			return;
			break;
	}

	


	if ( !CLUTStoreMode )
	{
		// CSM1 //
		//CLUT_LUT = ucCLUT_Lookup_CSM01_4bpp;
		
		if ( CLUTPixelFormat & 0x2 )
		{
			// 4-bit pixels - 16 colors
			// 16-bit pixels in CLUT //
			CLUTOffset &= 0x1f;
		}
		else
		{
			// 32-bit pixels in CLUT //
			CLUTOffset &= 0xf;
		}
	}
	else
	{
		// CSM2 //
		//CLUT_LUT = ucCLUT_Lookup_CSM02;
		
		// use clut_x, clut_y, clut_width
		//ptr_clut = & ( ptr_clut [ ( clut_x >> Shift3 ) + ( clut_y * ( clut_width >> Shift3 ) ) ] );
	}

	CLUTOffset <<= 4;
	ptr_clut16 = & ( InternalCLUT [ CLUTOffset ] );
	
	
	///////////////////////////////////
	// put top coordinates in x0,y0
	if ( xyz [ Coord1 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord0, Coord1 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord0 ].Y )
	{
		Swap ( Coord2, Coord0 );
	}
	
	if ( xyz [ Coord2 ].Y < xyz [ Coord1 ].Y )
	{
		Swap ( Coord2, Coord1 );
	}
	
	// get x,y
	x0 = xyz [ Coord0 ].X;
	y0 = xyz [ Coord0 ].Y;
	x1 = xyz [ Coord1 ].X;
	y1 = xyz [ Coord1 ].Y;
	x2 = xyz [ Coord2 ].X;
	y2 = xyz [ Coord2 ].Y;

	// get z
	z0 = (u64) xyz [ Coord0 ].Z;
	z1 = (u64) xyz [ Coord1 ].Z;
	z2 = (u64) xyz [ Coord2 ].Z;
	

	// get fog
	f0 = (u64) f [ Coord0 ].F;
	f1 = (u64) f [ Coord1 ].F;
	f2 = (u64) f [ Coord2 ].F;

	// get S
	fS0 = st [ Coord0 ].fS;
	fS1 = st [ Coord1 ].fS;
	fS2 = st [ Coord2 ].fS;
	
	// get T
	fT0 = st [ Coord0 ].fT;
	fT1 = st [ Coord1 ].fT;
	fT2 = st [ Coord2 ].fT;
	
	// get Q
	fQ0 = rgbaq [ Coord0 ].fQ;
	fQ1 = rgbaq [ Coord1 ].fQ;
	fQ2 = rgbaq [ Coord2 ].fQ;
	
	// prepare S,T,Q values
	fS0 = fS0 * ( TexWidth );
	fS1 = fS1 * ( TexWidth );
	fS2 = fS2 * ( TexWidth );
	
	fT0 = fT0 * ( TexHeight );
	fT1 = fT1 * ( TexHeight );
	fT2 = fT2 * ( TexHeight );
	
	
	
	//if ( GPURegsGp.PRIM.FST )
	if ( Fst )
	{
		// means using uv coords when FST is set //
		
		uv_temp [ Coord0 ].U = uv [ Coord0 ].U;
		uv_temp [ Coord0 ].V = uv [ Coord0 ].V;
		uv_temp [ Coord1 ].U = uv [ Coord1 ].U;
		uv_temp [ Coord1 ].V = uv [ Coord1 ].V;
		uv_temp [ Coord2 ].U = uv [ Coord2 ].U;
		uv_temp [ Coord2 ].V = uv [ Coord2 ].V;
		
	}
	else
	{
		// ST coords for triangles not implemented yet //


#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << dec << "; STCoords: s0=" << st [ Coord0 ].fS << " t0=" << st [ Coord0 ].fT << " s1=" << st [ Coord1 ].fS << " t1=" << st [ Coord1 ].fT << " s2=" << st [ Coord2 ].fS << " t2=" << st [ Coord2 ].fT;
	debug << " q0=" << rgbaq [ Coord0 ].fQ << " q1=" << rgbaq [ Coord1 ].fQ << " q2=" << rgbaq [ Coord2 ].fQ;
#endif

		// note: texture width/height should probably be minus one
		uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT / rgbaq [ Coord0 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT / rgbaq [ Coord1 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexWidth ) ) * 16.0f );
		uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT / rgbaq [ Coord2 ].fQ ) * ( (float) ( TexHeight ) ) * 16.0f );
		//uv_temp [ Coord0 ].U = (s32) ( ( st [ Coord0 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord0 ].V = (s32) ( ( st [ Coord0 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord1 ].U = (s32) ( ( st [ Coord1 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord1 ].V = (s32) ( ( st [ Coord1 ].fT ) * ( (float) TexHeight ) * 16.0f );
		//uv_temp [ Coord2 ].U = (s32) ( ( st [ Coord2 ].fS ) * ( (float) TexWidth ) * 16.0f );
		//uv_temp [ Coord2 ].V = (s32) ( ( st [ Coord2 ].fT ) * ( (float) TexHeight ) * 16.0f );
		
	}

	
#if defined INLINE_DEBUG_TRIANGLE || defined INLINE_DEBUG_PRIMITIVE
	debug << hex << " bgr=" << bgr;
	debug << dec << "; Coords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
	debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
#endif

	//////////////////////////////////////////
	// get coordinates on screen
	// *note* this is different from PS1, where you would add the offsets..
	x0 -= Coord_OffsetX;
	y0 -= Coord_OffsetY;
	x1 -= Coord_OffsetX;
	y1 -= Coord_OffsetY;
	x2 -= Coord_OffsetX;
	y2 -= Coord_OffsetY;
	
	
	
	// get the left/right most x
	LeftMostX = ( ( x0 < x1 ) ? x0 : x1 );
	LeftMostX = ( ( x2 < LeftMostX ) ? x2 : LeftMostX );
	RightMostX = ( ( x0 > x1 ) ? x0 : x1 );
	RightMostX = ( ( x2 > RightMostX ) ? x2 : RightMostX );
	
	LeftMostX >>= 4;
	RightMostX >>= 4;
	TopMostY = y0 >> 4;
	BottomMostY = y2 >> 4;

	// check if triangle is within draw area
	//if ( RightMostX <= ((s32)DrawArea_TopLeftX) || LeftMostX > ((s32)DrawArea_BottomRightX) || y2 <= ((s32)DrawArea_TopLeftY) || y0 > ((s32)DrawArea_BottomRightY) ) return;
	if ( RightMostX <= Window_XLeft || LeftMostX > Window_XRight || BottomMostY <= Window_YTop || TopMostY > Window_YBottom ) return;
#if defined INLINE_DEBUG_TRIANGLE_TEST || defined INLINE_DEBUG_PRIMITIVE_TEST
	else
	{
		debug << dec << "\r\nDrawTriangleGradientTexture";
		debug << dec << " FinalCoords: x0=" << ( x0 >> 4 ) << " y0=" << ( y0 >> 4 ) << " x1=" << ( x1 >> 4 ) << " y1=" << ( y1 >> 4 ) << " x2=" << ( x2 >> 4 ) << " y2=" << ( y2 >> 4 );
		debug << hex << " u0=" << uv_temp [ Coord0 ].U << " v0=" << uv_temp [ Coord0 ].V << " u1=" << uv_temp [ Coord1 ].U << " v1=" << uv_temp [ Coord1 ].V << " u2=" << uv_temp [ Coord2 ].U << " v2=" << uv_temp [ Coord2 ].V;
		debug << "; Clamp_ModeX=" << Clamp_ModeX << " Clamp_ModeY=" << Clamp_ModeY;
		debug << hex << "; CLUTBufPtr32/64=" << ( CLUTBufBase32 >> 6 );
		debug << "; CLUTPixFmt=" << PixelFormat_Names [ CLUTPixelFormat ];
		debug << hex << "; CLUTOffset/16=" << CLUTOffset;
		debug << "; CSM=" << CLUTStoreMode;
		debug << "; TEXCLUT=" << hex << GPURegsGp.TEXCLUT.Value;
		debug << hex << "; TexBufPtr32/64=" << ( TexBufStartOffset32 >> 6 );
		debug << dec << "; TexWidth=" << TexWidth << " TexHeight=" << TexHeight;
		debug << "; TexPixFmt=" << PixelFormat_Names [ PixelFormat ];
		debug << hex << "; FrameBufPtr32/64=" << ( FrameBufferStartOffset32 >> 6 );
		debug << " FrameBufPixFmt=" << PixelFormat_Names [ FrameBuffer_PixelFormat ];
		debug << " Alpha=" << Alpha;
		debug << " ZBUF=" << ZBUF_X.Value;
		debug << " MXL=" << TEX1 [ Ctx ].MXL;
		debug << " LCM=" << TEX1 [ Ctx ].LCM;
		debug << " K=" << TEX1 [ Ctx ].K;
	}
#endif
	
	// skip drawing if distance between vertices is greater than max allowed by GPU
	//if ( ( _Abs( x1 - x0 ) > c_MaxPolygonWidth ) || ( _Abs( x2 - x1 ) > c_MaxPolygonWidth ) || ( y1 - y0 > c_MaxPolygonHeight ) || ( y2 - y1 > c_MaxPolygonHeight ) )
	//{
	//	// skip drawing polygon
	//	return;
	//}
	
	
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// denominator is negative when x1 is on the left, positive when x1 is on the right
	s64 t0 = y1 - y2;
	s64 t1 = y0 - y2;
	s64 denominator = ( ( x0 - x2 ) * t0 ) - ( ( x1 - x2 ) * t1 );
	
	// check if x1 is on left or right //
	if ( denominator > 0 )
	{
		// x1 is on the right //
		X1Index = 1;
		X0Index = 0;
	}
	
	// calculate across
	if ( denominator )
	{
		//dudx = ( ( ( ( (s64) ( uv [ Coord0 ].U - uv [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].U - uv [ Coord2 ].U ) ) * t1 ) ) << 12 ) / denominator;
		//dvdx = ( ( ( ( (s64) ( uv [ Coord0 ].V - uv [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv [ Coord1 ].V - uv [ Coord2 ].V ) ) * t1 ) ) << 12 ) / denominator;
		dudx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].U - uv_temp [ Coord2 ].U ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].U - uv_temp [ Coord2 ].U ) ) * t1 ) ) << 16 ) / denominator;
		dvdx = ( ( ( ( (s64) ( uv_temp [ Coord0 ].V - uv_temp [ Coord2 ].V ) ) * t0 ) - ( ( (s64) ( uv_temp [ Coord1 ].V - uv_temp [ Coord2 ].V ) ) * t1 ) ) << 16 ) / denominator;
	
		dzdx = ( ( ( ( (s64) ( z0 - z2 ) ) * t0 ) - ( ( (s64) ( z1 - z2 ) ) * t1 ) ) << 27 ) / denominator;

		dfdx = ( ( ( ( (s64) ( f0 - f2 ) ) * t0 ) - ( ( (s64) ( f1 - f2 ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;

		dSdx = ( ( ( fS0 - fS2 ) * ( t0 / 16.0f ) ) - ( ( fS1 - fS2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dTdx = ( ( ( fT0 - fT2 ) * ( t0 / 16.0f ) ) - ( ( fT1 - fT2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		dQdx = ( ( ( fQ0 - fQ2 ) * ( t0 / 16.0f ) ) - ( ( fQ1 - fQ2 ) * ( t1 / 16.0f ) ) ) / ( denominator / 256.0f );
		
		drdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].R - rgbaq [ Coord2 ].R ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].R - rgbaq [ Coord2 ].R ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dgdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].G - rgbaq [ Coord2 ].G ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].G - rgbaq [ Coord2 ].G ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dbdx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].B - rgbaq [ Coord2 ].B ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].B - rgbaq [ Coord2 ].B ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
		dadx = ( ( ( ( (s64) ( rgbaq [ Coord0 ].A - rgbaq [ Coord2 ].A ) ) * t0 ) - ( ( (s64) ( rgbaq [ Coord1 ].A - rgbaq [ Coord2 ].A ) ) * t1 ) ) << ( 16 + 4 ) ) / denominator;
	}
	
	//debug << dec << "\r\nx0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
	//debug << "\r\nfixed: denominator=" << denominator;
	
	/////////////////////////////////////////////////
	// draw top part of triangle
	
	// need to set the x0 index unconditionally
	//x [ X0Index ] = ( ((s64)x0) << 32 );
	x [ X0Index ] = ( ((s64)x0) << 12 );
	
	//u [ X0Index ] = ( uv [ Coord0 ].U << 12 );
	//v [ X0Index ] = ( uv [ Coord0 ].V << 12 );
	u [ X0Index ] = ( uv_temp [ Coord0 ].U << 12 );
	v [ X0Index ] = ( uv_temp [ Coord0 ].V << 12 );
	
	fS [ X0Index ] = fS0;
	fT [ X0Index ] = fT0;
	fQ [ X0Index ] = fQ0;
	
	z [ X0Index ] = ( ((s64)z0) << 23 );
	fogc [ X0Index ] = ( ((s64)f0) << 16 );
	
	r [ X0Index ] = ( rgbaq [ Coord0 ].R << 16 );
	g [ X0Index ] = ( rgbaq [ Coord0 ].G << 16 );
	b [ X0Index ] = ( rgbaq [ Coord0 ].B << 16 );
	a [ X0Index ] = ( rgbaq [ Coord0 ].A << 16 );
	
	if ( y1 - y0 )
	{
		// triangle is pointed on top //
		
		/////////////////////////////////////////////
		// init x on the left and right
		//x_left = ( ((s64)x0) << 32 );
		//x_right = x_left;
		x [ X1Index ] = x [ X0Index ];
		
		u [ X1Index ] = u [ X0Index ];
		v [ X1Index ] = v [ X0Index ];
		
		fS [ X1Index ] = fS [ X0Index ];
		fT [ X1Index ] = fT [ X0Index ];
		fQ [ X1Index ] = fQ [ X0Index ];
		
		z [ X1Index ] = z [ X0Index ];
		fogc [ X1Index ] = fogc [ X0Index ];
		
		r [ X1Index ] = r [ X0Index ];
		g [ X1Index ] = g [ X0Index ];
		b [ X1Index ] = b [ X0Index ];
		a [ X1Index ] = a [ X0Index ];
		
		//dx_left = (((s64)( x1 - x0 )) << 32 ) / ((s64)( y1 - y0 ));
		//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
		dxdy [ X1Index ] = (((s64)( x1 - x0 )) << 16 ) / ((s64)( y1 - y0 ));
		dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
		
		//dudy [ X1Index ] = (((s32)( uv [ Coord1 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y1 - y0 ));
		//dvdy [ X1Index ] = (((s32)( uv [ Coord1 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y1 - y0 ));
		dudy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y1 - y0 ));
		dvdy [ X1Index ] = (((s32)( uv_temp [ Coord1 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y1 - y0 ));
		
		dSdy [ X1Index ] = ( fS1 - fS0 ) / ( ( y1 - y0 ) / 16.0f );
		dTdy [ X1Index ] = ( fT1 - fT0 ) / ( ( y1 - y0 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ1 - fQ0 ) / ( ( y1 - y0 ) / 16.0f );
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].R - rgbaq [ Coord0 ].R )) << 20 ) / ((s64)( y1 - y0 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].G - rgbaq [ Coord0 ].G )) << 20 ) / ((s64)( y1 - y0 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord1 ].B - rgbaq [ Coord0 ].B )) << 20 ) / ((s64)( y1 - y0 ));
		dady [ X1Index ] = (((s64)( rgbaq [ Coord1 ].A - rgbaq [ Coord0 ].A )) << 20 ) / ((s64)( y1 - y0 ));
		
		dzdy [ X1Index ] = (((s64)( z1 - z0 )) << 27 ) / ((s64)( y1 - y0 ));
		dfdy [ X1Index ] = (((s64)( f1 - f0 )) << 20 ) / ((s64)( y1 - y0 ));
		
		//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
		//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
		dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
		dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));

		dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
		dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
		dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
		
		drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 20 ) / ((s64)( y2 - y0 ));
		dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 20 ) / ((s64)( y2 - y0 ));
		dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 20 ) / ((s64)( y2 - y0 ));
		dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 20 ) / ((s64)( y2 - y0 ));
		
		dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
		dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
	}
	else
	{
		// Triangle is flat on top //
		
		// change x_left and x_right where y1 is on left
		//x [ X1Index ] = ( ((s64)x1) << 32 );
		x [ X1Index ] = ( ((s64)x1) << 12 );
		
		//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
		//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
		u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
		v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );

		fS [ X1Index ] = fS1;
		fT [ X1Index ] = fT1;
		fQ [ X1Index ] = fQ1;
		
		z [ X1Index ] = ( ((s64)z1) << 23 );
		fogc [ X1Index ] = ( ((s64)f1) << 16 );
		
		r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
		g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
		b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );
		a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
		
		// this means that x0 and x1 are on the same line
		// so if the height of the entire polygon is zero, then we are done
		if ( y2 - y1 )
		{
			//dx_left = (((s64)( x2 - x1 )) << 32 ) / ((s64)( y2 - y1 ));
			//dx_right = (((s64)( x2 - x0 )) << 32 ) / ((s64)( y2 - y0 ));
			dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
			dxdy [ X0Index ] = (((s64)( x2 - x0 )) << 16 ) / ((s64)( y2 - y0 ));
			
			//dudy [ X0Index ] = (((s32)( uv [ Coord2 ].U - uv [ Coord0 ].U )) << 12 ) / ((s32)( y2 - y0 ));
			//dvdy [ X0Index ] = (((s32)( uv [ Coord2 ].V - uv [ Coord0 ].V )) << 12 ) / ((s32)( y2 - y0 ));
			dudy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].U - uv_temp [ Coord0 ].U )) << 16 ) / ((s32)( y2 - y0 ));
			dvdy [ X0Index ] = (((s32)( uv_temp [ Coord2 ].V - uv_temp [ Coord0 ].V )) << 16 ) / ((s32)( y2 - y0 ));
			
			dSdy [ X0Index ] = ( fS2 - fS0 ) / ( ( y2 - y0 ) / 16.0f );
			dTdy [ X0Index ] = ( fT2 - fT0 ) / ( ( y2 - y0 ) / 16.0f );
			dQdy [ X0Index ] = ( fQ2 - fQ0 ) / ( ( y2 - y0 ) / 16.0f );
			
			dzdy [ X0Index ] = (((s64)( z2 - z0 )) << 27 ) / ((s64)( y2 - y0 ));
			dfdy [ X0Index ] = (((s64)( f2 - f0 )) << 20 ) / ((s64)( y2 - y0 ));
			
			// only need to set dr,dg,db for the x0/x2 side here
			drdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord0 ].R )) << 20 ) / ((s64)( y2 - y0 ));
			dgdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord0 ].G )) << 20 ) / ((s64)( y2 - y0 ));
			dbdy [ X0Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord0 ].B )) << 20 ) / ((s64)( y2 - y0 ));
			dady [ X0Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord0 ].A )) << 20 ) / ((s64)( y2 - y0 ));
		}
	}
	
	
	
	switch ( Clamp_ModeY )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexY_And = TexHeight_Mask;
			
			TexY_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexY_Min = 0;
			TexY_Max = TexHeight_Mask;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexY_Min = Clamp_MinV;
			TexY_Max = Clamp_MaxV;
			
			TexY_And = TexHeight_Mask;
			TexY_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexY_And = Clamp_MinV & TexHeight_Mask;
			TexY_Or = Clamp_MaxV & TexHeight_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexY_Min = -2047;
			TexY_Max = 2047;
			break;
	}

	
	switch ( Clamp_ModeX )
	{
		case 0:
			// repeat //
			//TexCoordY &= TexHeight_Mask;
			TexX_And = TexWidth_Mask;
			
			TexX_Or = 0;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
			
		case 1:
			// clamp //
			//TexCoordY = ( TexCoordY < 0 ) ? 0 : TexCoordY;
			//TexCoordY = ( TexCoordY > TexHeight ) ? TexHeight : TexCoordY;
			TexX_Min = 0;
			TexX_Max = TexWidth_Mask;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 2:
			// region clamp //
			//TexCoordY = ( TexCoordY < Clamp_MinV ) ? Clamp_MinV : TexCoordY;
			//TexCoordY = ( TexCoordY > Clamp_MaxV ) ? Clamp_MaxV : TexCoordY;
			TexX_Min = Clamp_MinU;
			TexX_Max = Clamp_MaxU;
			
			TexX_And = TexWidth_Mask;
			TexX_Or = 0;
			break;
			
		case 3:
			// region repeat //
			// this one is just like on the ps1
			//TexCoordY = ( TexCoordY & Clamp_MinV ) | Clamp_MaxV;
			TexX_And = Clamp_MinU & TexWidth_Mask;
			TexX_Or = Clamp_MaxU & TexWidth_Mask;
			
			// texture coords are clamped outside of -2047 to +2047
			TexX_Min = -2047;
			TexX_Max = 2047;
			break;
	}
	

	// left point is included if points are equal
	StartY = ( (s64) ( y0 + 0xf ) ) >> 4;
	EndY = ( (s64) ( y1 - 1 ) ) >> 4;
	
	Temp = ( StartY << 4 ) - y0;
	

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp += ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp += ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
	}
	
	// dxdy is in .16, Temp is in .4, and x is in .16
	x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
	x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
	
	u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
	v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
	
	fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
	fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
	fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
	
	r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
	g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
	b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
	a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
	
	z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
	fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
	
	
	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	
	
	//vAlphaSelect [ 0 ] = _mm256_set1_epi32 ( AlphaSelect [ 0 ] );
	vAlphaSelect [ 2 ] = _mm256_set1_epi32 ( AlphaSelect [ 2 ] );
	vAlphaSelect [ 3 ] = _mm256_set1_epi32 ( AlphaSelect [ 3 ] );

	
	vZIdentity0 = _mm256_set_epi64x ( dzdx * 6, dzdx * 4, dzdx * 2, 0 );
	vZIdentity1 = _mm256_set_epi64x ( dzdx * 7, dzdx * 5, dzdx * 3, dzdx * 1 );
	vZInc = _mm256_set1_epi64x ( dzdx * c_iVectorSize );

	vTexBufWidth = _mm256_set1_epi32 ( TexBufWidth );
	
	vTexY_Min = _mm256_set1_epi32 ( TexY_Min );
	vTexY_Max = _mm256_set1_epi32 ( TexY_Max );
	vTexY_And = _mm256_set1_epi32 ( TexY_And );
	vTexY_Or = _mm256_set1_epi32 ( TexY_Or );
	
	vTexX_Min = _mm256_set1_epi32 ( TexX_Min );
	vTexX_Max = _mm256_set1_epi32 ( TexX_Max );
	vTexX_And = _mm256_set1_epi32 ( TexX_And );
	vTexX_Or = _mm256_set1_epi32 ( TexX_Or );
	
	//vc1 = _mm256_set1_epi32 ( c1 );
	//vc2 = _mm256_set1_epi32 ( c2 );
	//vc3 = _mm256_set1_epi32 ( c3 );
	//vca = _mm256_set1_epi32 ( ca );

	vUInc = _mm256_set1_epi32 ( dudx * c_iVectorSize );
	vVInc = _mm256_set1_epi32 ( dvdx * c_iVectorSize );
	
	vFInc = _mm256_set1_epi32 ( dfdx * c_iVectorSize );
	
	vRInc = _mm256_set1_epi32 ( drdx * c_iVectorSize );
	vGInc = _mm256_set1_epi32 ( dgdx * c_iVectorSize );
	vBInc = _mm256_set1_epi32 ( dbdx * c_iVectorSize );
	vAInc = _mm256_set1_epi32 ( dadx * c_iVectorSize );
	
	vSInc = _mm256_set1_ps ( dSdx * c_iVectorSize );
	vTInc = _mm256_set1_ps ( dTdx * c_iVectorSize );
	vQInc = _mm256_set1_ps ( dQdx * c_iVectorSize );
	
	vUIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dudx ), vIdentity );
	vVIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dvdx ), vIdentity );
	
	vFIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dfdx ), vIdentity );
	
	vRIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( drdx ), vIdentity );
	vGIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dgdx ), vIdentity );
	vBIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dbdx ), vIdentity );
	vAIdentity = _mm256_mullo_epi32 ( _mm256_set1_epi32 ( dadx ), vIdentity );

	vSIdentity = _mm256_mul_ps ( _mm256_set1_ps ( dSdx ), vIdentityf );
	vTIdentity = _mm256_mul_ps ( _mm256_set1_ps ( dTdx ), vIdentityf );
	vQIdentity = _mm256_mul_ps ( _mm256_set1_ps ( dQdx ), vIdentityf );
	
	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y1
	for ( Line = StartY; Line <= EndY; Line++ )
	{

		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		

		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];

			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			iA = a [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );

			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vYacross = _mm256_set1_epi32 ( Line );
			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iF ), vFIdentity );
			
			vRacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iA ), vAIdentity );
			
			vSacross = _mm256_add_ps ( _mm256_set1_ps ( iS ), vSIdentity );
			vTacross = _mm256_add_ps ( _mm256_set1_ps ( iT ), vTIdentity );
			vQacross = _mm256_add_ps ( _mm256_set1_ps ( iQ ), vQIdentity );


			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm256_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm256_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm256_cvttps_epi32 ( _mm256_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm256_cvttps_epi32 ( _mm256_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm256_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm256_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm256_and_si256 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm256_or_si256 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm256_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm256_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm256_and_si256 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm256_or_si256 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix8x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 24 ), 24 );
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						vTexOffset32 = CvtAddrPix4x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm256_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 28 ), 28 );
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 24 );
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 4 ), 28 );
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 28 );
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					//bgr = ptr_clut16 [ bgr_temp ];
					
					vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
						
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
								
						vBgr2 = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						vBgr = _mm256_or_si256 ( vBgr, _mm256_slli_epi32 ( vBgr2, 16 ) );
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 + 3 ) ), _mm256_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm256_or_si256 ( vBgr, _mm256_blendv_epi8 ( vTA0, vTA1, _mm256_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm256_or_si256 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				vc1 = _mm256_srli_epi32 ( vRacross, 16 );
				vc2 = _mm256_srli_epi32 ( vGacross, 16 );
				vc3 = _mm256_srli_epi32 ( vBacross, 16 );
				vca = _mm256_srli_epi32 ( vAacross, 16 );
				
				// texture function ??
				vBgr = TextureFunc32x8 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vF = _mm256_srli_epi32 ( vFacross, 16 );
					vBgr = FogFunc32x8 ( vBgr, vF );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;

				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );

				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
						
				iU += dudx;
				iV += dvdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				
				iZ += dzdx;
				iF += dfdx;
				
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm256_add_epi32 ( vUacross, vUInc );
				vVacross = _mm256_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm256_add_epi32 ( vFacross, vFInc );
				
				vRacross = _mm256_add_epi32 ( vRacross, vRInc );
				vGacross = _mm256_add_epi32 ( vGacross, vGInc );
				vBacross = _mm256_add_epi32 ( vBacross, vBInc );
				vAacross = _mm256_add_epi32 ( vAacross, vAInc );
				
				vSacross = _mm256_add_ps ( vSacross, vSInc );
				vTacross = _mm256_add_ps ( vTacross, vTInc );
				vQacross = _mm256_add_ps ( vQacross, vQInc );
				
			}
			
		}
		
		//////////////////////////////////
		// draw next line
		//Line++;
		
		/////////////////////////////////////
		// update x on left and right
		//x_left += dx_left;
		//x_right += dx_right;
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		a [ 0 ] += dady [ 0 ];
		
	}
	
	} // if ( EndY >= StartY )

	
	////////////////////////////////////////////////
	// draw bottom part of triangle

	/////////////////////////////////////////////
	// init x on the left and right
	
	// set values on the x1 side
	//x [ X1Index ] = ( ((s64)x1) << 32 );
	x [ X1Index ] = ( ((s64)x1) << 12 );
	
	//u [ X1Index ] = ( uv [ Coord1 ].U << 12 );
	//v [ X1Index ] = ( uv [ Coord1 ].V << 12 );
	u [ X1Index ] = ( uv_temp [ Coord1 ].U << 12 );
	v [ X1Index ] = ( uv_temp [ Coord1 ].V << 12 );
	
	fS [ X1Index ] = fS1;
	fT [ X1Index ] = fT1;
	fQ [ X1Index ] = fQ1;
	
	z [ X1Index ] = ( ((s64)z1) << 23 );
	fogc [ X1Index ] = ( ((s64)f1) << 16 );
	
	r [ X1Index ] = ( rgbaq [ Coord1 ].R << 16 );
	g [ X1Index ] = ( rgbaq [ Coord1 ].G << 16 );
	b [ X1Index ] = ( rgbaq [ Coord1 ].B << 16 );
	a [ X1Index ] = ( rgbaq [ Coord1 ].A << 16 );
	
	if ( y2 - y1 )
	{
		// triangle is pointed on the bottom //
		dxdy [ X1Index ] = (((s64)( x2 - x1 )) << 16 ) / ((s64)( y2 - y1 ));
		
		//dudy [ X1Index ] = (((s64)( uv [ Coord2 ].U - uv [ Coord1 ].U )) << 12 ) / ((s64)( y2 - y1 ));
		//dvdy [ X1Index ] = (((s64)( uv [ Coord2 ].V - uv [ Coord1 ].V )) << 12 ) / ((s64)( y2 - y1 ));
		dudy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].U - uv_temp [ Coord1 ].U )) << 16 ) / ((s64)( y2 - y1 ));
		dvdy [ X1Index ] = (((s64)( uv_temp [ Coord2 ].V - uv_temp [ Coord1 ].V )) << 16 ) / ((s64)( y2 - y1 ));
		
		dSdy [ X1Index ] = ( fS2 - fS1 ) / ( ( y2 - y1 ) / 16.0f );
		dTdy [ X1Index ] = ( fT2 - fT1 ) / ( ( y2 - y1 ) / 16.0f );
		dQdy [ X1Index ] = ( fQ2 - fQ1 ) / ( ( y2 - y1 ) / 16.0f );
		
		dzdy [ X1Index ] = (((s64)( z2 - z1 )) << 27 ) / ((s64)( y2 - y1 ));
		dfdy [ X1Index ] = (((s64)( f2 - f1 )) << 20 ) / ((s64)( y2 - y1 ));
		
		drdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].R - rgbaq [ Coord1 ].R )) << 20 ) / ((s64)( y2 - y1 ));
		dgdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].G - rgbaq [ Coord1 ].G )) << 20 ) / ((s64)( y2 - y1 ));
		dbdy [ X1Index ] = (((s64)( rgbaq [ Coord2 ].B - rgbaq [ Coord1 ].B )) << 20 ) / ((s64)( y2 - y1 ));
		dady [ X1Index ] = (((s64)( rgbaq [ Coord2 ].A - rgbaq [ Coord1 ].A )) << 20 ) / ((s64)( y2 - y1 ));
	}
	
	
	// *** testing ***
	//debug << "\r\nfixed: dR_across=" << dR_across << " dG_across=" << dG_across << " dB_across=" << dB_across;
	
	// the line starts at y1 from here
	//Line = y1;

	//StartY = y1;
	//EndY = y2;

	// left point is included if points are equal
	StartY = ( y1 + 0xf ) >> 4;
	EndY = ( y2 - 1 ) >> 4;
	
	Temp = ( StartY << 4 ) - y1;
	
	// update the values on the X1 side to the next pixel
	x [ X1Index ] += ( dxdy [ X1Index ] >> 4 ) * Temp;
	
	u [ X1Index ] += ( dudy [ X1Index ] >> 4 ) * Temp;
	v [ X1Index ] += ( dvdy [ X1Index ] >> 4 ) * Temp;

	fS [ X1Index ] += ( dSdy [ X1Index ] ) * ( Temp / 16.0f );
	fT [ X1Index ] += ( dTdy [ X1Index ] ) * ( Temp / 16.0f );
	fQ [ X1Index ] += ( dQdy [ X1Index ] ) * ( Temp / 16.0f );
	
	r [ X1Index ] += ( drdy [ X1Index ] >> 4 ) * Temp;
	g [ X1Index ] += ( dgdy [ X1Index ] >> 4 ) * Temp;
	b [ X1Index ] += ( dbdy [ X1Index ] >> 4 ) * Temp;
	a [ X1Index ] += ( dady [ X1Index ] >> 4 ) * Temp;
	
	z [ X1Index ] += ( dzdy [ X1Index ] >> 4 ) * Temp;
	fogc [ X1Index ] += ( dfdy [ X1Index ] >> 4 ) * Temp;

	if ( StartY < Window_YTop )
	{
		if ( EndY < Window_YTop )
		{
			//Temp = EndY - StartY;
			Temp = ( EndY - StartY + 1 ) << 4;
			//StartY = EndY;
			StartY = EndY + 1;
		}
		else
		{
			Temp = ( Window_YTop - StartY ) << 4;
			StartY = Window_YTop;
		}
		
		// dxdy is in .16, Temp is in .4, and x is in .16
		x [ 0 ] += ( dxdy [ 0 ] >> 4 ) * Temp;
		x [ 1 ] += ( dxdy [ 1 ] >> 4 ) * Temp;
		
		u [ 0 ] += ( dudy [ 0 ] >> 4 ) * Temp;
		v [ 0 ] += ( dvdy [ 0 ] >> 4 ) * Temp;
		
		fS [ 0 ] += ( dSdy [ 0 ] ) * ( Temp / 16.0f );
		fT [ 0 ] += ( dTdy [ 0 ] ) * ( Temp / 16.0f );
		fQ [ 0 ] += ( dQdy [ 0 ] ) * ( Temp / 16.0f );
		
		r [ 0 ] += ( drdy [ 0 ] >> 4 ) * Temp;
		g [ 0 ] += ( dgdy [ 0 ] >> 4 ) * Temp;
		b [ 0 ] += ( dbdy [ 0 ] >> 4 ) * Temp;
		a [ 0 ] += ( dady [ 0 ] >> 4 ) * Temp;
		
		z [ 0 ] += ( dzdy [ 0 ] >> 4 ) * Temp;
		fogc [ 0 ] += ( dfdy [ 0 ] >> 4 ) * Temp;
		
	}
	

	
	if ( EndY > Window_YBottom )
	{
		//EndY = Window_YBottom + 1;
		EndY = Window_YBottom;
	}

	
	if ( EndY >= StartY )
	{
	
	//////////////////////////////////////////////
	// draw down to y2
	for ( Line = StartY; Line <= EndY; Line++ )
	{
		// left point is included if points are equal
		StartX = ( x [ 0 ] + 0xffffLL ) >> 16;
		EndX = ( x [ 1 ] - 1 ) >> 16;
		
		
		if ( StartX <= Window_XRight && EndX >= Window_XLeft && EndX >= StartX )
		{
			iU = u [ 0 ];
			iV = v [ 0 ];
			
			iS = fS [ 0 ];
			iT = fT [ 0 ];
			iQ = fQ [ 0 ];
			
			iR = r [ 0 ];
			iG = g [ 0 ];
			iB = b [ 0 ];
			iA = a [ 0 ];
			
			iZ = z [ 0 ];
			iF = fogc [ 0 ];
			
		
			Temp = ( StartX << 16 ) - x [ 0 ];
			
			if ( StartX < Window_XLeft )
			{
				Temp += ( Window_XLeft - StartX ) << 16;
				StartX = Window_XLeft;
			}
			
			iU += ( dudx >> 8 ) * ( Temp >> 8 );
			iV += ( dvdx >> 8 ) * ( Temp >> 8 );
			
			iS += ( dSdx ) * ( Temp / 65536.0f );
			iT += ( dTdx ) * ( Temp / 65536.0f );
			iQ += ( dQdx ) * ( Temp / 65536.0f );
			
			iR += ( drdx >> 8 ) * ( Temp >> 8 );
			iG += ( dgdx >> 8 ) * ( Temp >> 8 );
			iB += ( dbdx >> 8 ) * ( Temp >> 8 );
			iA += ( dadx >> 8 ) * ( Temp >> 8 );
			
			iZ += ( dzdx >> 8 ) * ( Temp >> 8 );
			iF += ( dfdx >> 8 ) * ( Temp >> 8 );
			
			if ( EndX > Window_XRight )
			{
				//EndX = Window_XRight + 1;
				EndX = Window_XRight;
			}
			
			
			
			
			/////////////////////////////////////////////////////
			// update number of cycles used to draw polygon
			NumberOfPixelsDrawn += EndX - StartX + 1;
			

			vYacross = _mm256_set1_epi32 ( Line );
			vXacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( StartX ), vIdentity );
			vXmax = _mm256_set1_epi32 ( EndX + 1 );
			
			vZacross0 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity0 );
			vZacross1 = _mm256_add_epi64 ( _mm256_set1_epi64x ( iZ ), vZIdentity1 );
			
			vUacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iU ), vUIdentity );
			vVacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iV ), vVIdentity );
			
			vFacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iF ), vFIdentity );
			
			vRacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iR ), vRIdentity );
			vGacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iG ), vGIdentity );
			vBacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iB ), vBIdentity );
			vAacross = _mm256_add_epi32 ( _mm256_set1_epi32 ( iA ), vAIdentity );
			
			vSacross = _mm256_add_ps ( _mm256_set1_ps ( iS ), vSIdentity );
			vTacross = _mm256_add_ps ( _mm256_set1_ps ( iT ), vTIdentity );
			vQacross = _mm256_add_ps ( _mm256_set1_ps ( iQ ), vQIdentity );

			// draw horizontal line
			// x_left and x_right need to be rounded off
			//for ( x_across = StartX; x_across < EndX; x_across++ )
			for ( x_across = StartX; x_across <= EndX /* && ptr16 < PtrEnd */; x_across += c_iVectorSize )
			{
				// check what pixels are enabled based on x position
				vEnable = _mm256_cmpgt_epi32 ( vXmax, vXacross );
				
				//TexCoordX = ( iU >> 16 );
				//TexCoordY = ( iV >> 16 );
				vTexCoordX = _mm256_srai_epi32 ( vUacross, 16 );
				vTexCoordY = _mm256_srai_epi32 ( vVacross, 16 );

#ifdef ENABLE_3D_TMAPPING_FTRI
				if ( !Fst )
				{
					//TexCoordX = (long) (iS / iQ);
					//TexCoordY = (long) (iT / iQ);
					vTexCoordX = _mm256_cvttps_epi32 ( _mm256_div_ps ( vSacross, vQacross ) );
					vTexCoordY = _mm256_cvttps_epi32 ( _mm256_div_ps ( vTacross, vQacross ) );
				}
#endif

				//TexCoordY = ( ( TexCoordY < TexY_Min ) ? TexY_Min : TexCoordY );
				//TexCoordY = ( ( TexCoordY > TexY_Max ) ? TexY_Max : TexCoordY );
				//TexCoordY &= TexY_And;
				//TexCoordY |= TexY_Or;

				//TexCoordX = ( ( TexCoordX < TexX_Min ) ? TexX_Min : TexCoordX );
				//TexCoordX = ( ( TexCoordX > TexX_Max ) ? TexX_Max : TexCoordX );
				//TexCoordX &= TexX_And;
				//TexCoordX |= TexX_Or;

				vTexCoordY = _mm256_max_epi32 ( vTexCoordY, vTexY_Min );
				vTexCoordY = _mm256_min_epi32 ( vTexCoordY, vTexY_Max );
				vTexCoordY = _mm256_and_si256 ( vTexCoordY, vTexY_And );
				vTexCoordY = _mm256_or_si256 ( vTexCoordY, vTexY_Or );
					
				vTexCoordX = _mm256_max_epi32 ( vTexCoordX, vTexX_Min );
				vTexCoordX = _mm256_min_epi32 ( vTexCoordX, vTexX_Max );
				vTexCoordX = _mm256_and_si256 ( vTexCoordX, vTexX_And );
				vTexCoordX = _mm256_or_si256 ( vTexCoordX, vTexX_Or );


				// get texel from texture buffer (32-bit frame buffer) //

				switch ( PixelFormat )
				{
					// PSMCT32
					case 0:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						//break;
						
					// PSMCT24
					case 1:
						//bgr = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );

						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMCT16
					case 2:
						//bgr = ptr_texture16 [ CvtAddrPix16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMCT16S
					case 0xa:
						//bgr = ptr_texture16 [ CvtAddrPix16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMT8
					case 0x13:
						//bgr_temp = ptr_texture8 [ CvtAddrPix8 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrPix8x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 24 ), 24 );
						break;
						
					// PSMT4
					case 0x14:
						//bgr = CvtAddrPix4 ( TexCoordX, TexCoordY, TexBufWidth );
						//bgr_temp = ( ( ptr_texture8 [ bgr >> 1 ] ) >> ( ( bgr & 1 ) << 2 ) ) & 0xf;
						vTexOffset32 = CvtAddrPix4x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vTexOffset32, 31 ), 31 - 2 );
						vTexOffset32 = _mm256_srli_epi32 ( vTexOffset32, 1 );
						
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 1 );
						vBgr_temp = _mm256_srlv_epi32 ( vBgr_temp, vBgr );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 28 ), 28 );
						break;
						
					// PSMT8H
					case 0x1b:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] ) >> 24;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 24 );
						break;
						
					// PSMT4HL
					case 0x24:
						//bgr_temp = ( ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 24 ) & 0xf;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr_temp, 4 ), 28 );
						break;
						
					// PSMT4HH
					case 0x2c:
						//bgr_temp = ptr_texture32 [ CvtAddrPix32 ( TexCoordX, TexCoordY, TexBufWidth ) ] >> 28;
						vTexOffset32 = CvtAddrPix32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr_temp = _mm256_mask_i32gather_epi32 ( vBgr_temp, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						vBgr_temp = _mm256_srli_epi32 ( vBgr_temp, 28 );
						break;
						
					// Z-buffer formats
					
					// PSMZ32
					case 0x30:
					
					// PSMZ24
					case 0x31:
					//case 0x35:
						//bgr = ptr_texture32 [ CvtAddrZBuf32 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf32x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 4 );
						break;
						
					// PSMZ16
					case 0x32:
						//bgr = ptr_texture16 [ CvtAddrZBuf16 ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16x8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
						
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
						
					// PSMZ16S
					case 0x3a:
						//bgr = ptr_texture16 [ CvtAddrZBuf16S ( TexCoordX, TexCoordY, TexBufWidth ) ];
						vTexOffset32 = CvtAddrZBuf16Sx8 ( vTexCoordX, vTexCoordY, vTexBufWidth, vEnable );
								
						vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_texture32, vTexOffset32, vEnable, 2 );
						break;
				}
				
				// look up color value in CLUT if needed //
				if ( ( PixelFormat & 7 ) >= 3 )
				{
					// lookup color value in CLUT //
					
					//bgr = ptr_clut16 [ bgr_temp ];
					
					vBgr = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ptr_clut16, vBgr_temp, vEnable, 2 );
						
					if ( ! ( CLUTPixelFormat & 0x2 ) )
					{
						// 32-bit pixels in CLUT //
						//bgr |= ( (u32) ptr_clut16 [ bgr_temp + 256 ] ) << 16;
								
						vBgr2 = _mm256_mask_i32gather_epi32 ( vBgr, (int const*) ( ptr_clut16 + 256 ), vBgr_temp, vEnable, 2 );
						vBgr = _mm256_or_si256 ( vBgr, _mm256_slli_epi32 ( vBgr2, 16 ) );
					}
				}
				
				
				// convert 16-bit pixels to 32-bit (since 32-bit frame buffer here) //
				
				// check if pixel is 16-bit
				if ( ( ( PixelFormat == 0x2 ) || ( PixelFormat == 0xa ) ) || ( ( PixelFormat > 0xa ) && ( CLUTPixelFormat & 0x2 ) ) )
				{
					// pixel is 16-bit //
					
					// get alpha component of 16-bit pixel
					//bgr_temp = bgr & 0x8000;
					vBgr_temp = vBgr;
					
					// convert to 32-bit
					//bgr = ConvertPixel16To24 ( bgr );
					// convert 16-bit pixels to 32-bit pixels
					vRed = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 24 + 3 ), 24 );
					vGreen = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 16 + 6 ), 24 + 3 );
					vBlue = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 + 9 ), 24 + 3 );
					vBgr = _mm256_or_si256 ( _mm256_or_si256 ( vRed, _mm256_slli_epi32 ( vGreen, 8 + 3 ) ), _mm256_slli_epi32 ( vBlue, 16 + 3 ) );
					
					
					// set the texture alpha for 16 bit pixel
					//bgr |= ( ( ( bgr_temp & 0x8000 ) ? GPURegsGp.TEXA.TA1 : GPURegsGp.TEXA.TA0 ) << 24 );
					vBgr = _mm256_or_si256 ( vBgr, _mm256_blendv_epi8 ( vTA0, vTA1, _mm256_slli_epi32 ( vBgr_temp, 16 ) ) );

					
					// check if pixel is definitely not transparent, if transparent then just stays zero
					//if ( ( !GPURegsGp.TEXA.AEM ) || bgr )
					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_setzero_si256 () ), vBgr );
					}
					
				}
				
				
				if ( PixelFormat == 1 )
				{
					// 24-bit pixel //
					//bgr &= 0xffffff;
					vBgr = _mm256_srli_epi32 ( _mm256_slli_epi32 ( vBgr, 8 ), 8 );

					vBgr_temp = vBgr;
					
					vBgr = _mm256_or_si256 ( vBgr, vTA0 );
					

					if ( GPURegsGp.TEXA.AEM )
					{
						// if pixel is zero, then clear it and it is transparent
						vBgr = _mm256_andnot_si256 ( _mm256_cmpeq_epi32 ( vBgr_temp, _mm256_set1_epi32 ( 0 ) ), vBgr );
					}
					
				}
				
				vc1 = _mm256_srli_epi32 ( vRacross, 16 );
				vc2 = _mm256_srli_epi32 ( vGacross, 16 );
				vc3 = _mm256_srli_epi32 ( vBacross, 16 );
				vca = _mm256_srli_epi32 ( vAacross, 16 );
				
				// texture function ??
				vBgr = TextureFunc32x8 ( vBgr, vc1, vc2, vc3, vca );
				
				// fog function ??
				if ( FogEnable )
				{
					vF = _mm256_srli_epi32 ( vFacross, 16 );
					vBgr = FogFunc32x8 ( vBgr, vF );
				}
				
				
				vAlphaSelect [ 0 ] = vBgr;

				vZacross = _mm256_blend_epi16 ( _mm256_srli_epi64 ( vZacross0, 23 ), _mm256_slli_epi64 ( vZacross1, 9 ), 0xcc );

				// no need to interpolate Z-value for sprite? just use z1 value?
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr );
				//PlotPixel_Texture ( x_across, Line, iZ >> 23, bgr, c1, c2, c3, ca, iF >> 24 );
				PlotPixel_Monox8 ( vXacross, vYacross, vZacross, vBgr, vEnable );
				
						
				iU += dudx;
				iV += dvdx;
				
				iS += dSdx;
				iT += dTdx;
				iQ += dQdx;
				
				iR += drdx;
				iG += dgdx;
				iB += dbdx;
				iA += dadx;
				
				iZ += dzdx;
				iF += dfdx;
				
				vXacross = _mm256_add_epi32 ( vXacross, vXInc );
				
				vZacross0 = _mm256_add_epi64 ( vZacross0, vZInc );
				vZacross1 = _mm256_add_epi64 ( vZacross1, vZInc );
				
				vUacross = _mm256_add_epi32 ( vUacross, vUInc );
				vVacross = _mm256_add_epi32 ( vVacross, vVInc );
				
				vFacross = _mm256_add_epi32 ( vFacross, vFInc );
				
				vRacross = _mm256_add_epi32 ( vRacross, vRInc );
				vGacross = _mm256_add_epi32 ( vGacross, vGInc );
				vBacross = _mm256_add_epi32 ( vBacross, vBInc );
				vAacross = _mm256_add_epi32 ( vAacross, vAInc );
				
				vSacross = _mm256_add_ps ( vSacross, vSInc );
				vTacross = _mm256_add_ps ( vTacross, vTInc );
				vQacross = _mm256_add_ps ( vQacross, vQInc );
			}
			
		}
		
		/////////////////////////////////////
		// update x on left and right
		x [ 0 ] += dxdy [ 0 ];
		x [ 1 ] += dxdy [ 1 ];
		
		u [ 0 ] += dudy [ 0 ];
		v [ 0 ] += dvdy [ 0 ];
		
		fS [ 0 ] += dSdy [ 0 ];
		fT [ 0 ] += dTdy [ 0 ];
		fQ [ 0 ] += dQdy [ 0 ];
		
		r [ 0 ] += drdy [ 0 ];
		g [ 0 ] += dgdy [ 0 ];
		b [ 0 ] += dbdy [ 0 ];
		a [ 0 ] += dady [ 0 ];
		
		z [ 0 ] += dzdy [ 0 ];
		fogc [ 0 ] += dfdy [ 0 ];
		
	}
	
	} // if ( EndY >= StartY )

}

#endif




void GPU::TransferDataLocal_DS ()
{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
	if ( !XferX && !XferY )
	{
		debug << "\r\nTransferLocal: ";
		//debug << dec << " WC=" << WordCount32;
		debug << dec << " Width=" << XferWidth << " Height=" << XferHeight;
		debug << hex << " DSTPTR32/64=" << GPURegsGp.BITBLTBUF.DBP;
		debug << hex << " SRCPTR32/64=" << GPURegsGp.BITBLTBUF.SBP;
		debug << dec << " DstBufWidth=" << XferDstBufWidth;
		debug << dec << " SrcBufWidth=" << XferSrcBufWidth;
		debug << " OutPixFmt=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.DPSM ];
		debug << " InPixFmt=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.SPSM ];
		debug << " TransferDir=" << TransferDir_Names [ GPURegsGp.TRXPOS.DIR ];
		debug << dec << " XferX=" << XferX << " XferY=" << XferY;
		debug << dec << " XferDstX=" << XferDstX << " XferDstY=" << XferDstY;
		debug << dec << " XferSrcX=" << XferSrcX << " XferSrcY=" << XferSrcY;
		debug << " @Cycle#" << dec << *_DebugCycleCount;
#ifdef VERBOSE_TRANSFER_LOCAL
		cout << "\nhps2x64: ALERT: GPU: Transferring data TO gpu FROM gpu";
#endif
	}
#endif

#ifdef INLINE_DEBUG_TRANSFER_LOCAL_2
	debug << "\r\n";
	debug << " XferX=" << dec << XferX << " XferY=" << XferY;
	//debug << " WC=" << WordCount32;
	debug << " Offset=" << ( ( XferSrcOffset32 + XferX + ( XferY * XferSrcBufWidth ) ) << 2 );
	debug << " SrcPage#" << ( ( XferSrcOffset32 + XferX + ( XferY * XferSrcBufWidth ) ) >> 11 );
	debug << " DstPage#" << ( ( XferDstOffset32 + XferX + ( XferY * XferDstBufWidth ) ) >> 11 );
	//debug << " XferDstX=" << XferDstX << " XferDstY=" << XferDstY;
#endif

	// all transfer coords wraparound
	const u32 c_ulTransferXMask = 0x7ff;

	//u64 PixelShift
	u64 PixelLoad;
	u32 Pixel0;
	u32 Pixel1;
	//u64 PixelCount;

	u32 CurSrcX, CurSrcY;
	u32 CurDstX, CurDstY;

	// get pointer to dest buffer
	//u32 *DestBuffer;
	
	u32 *Data32;
	u16 *Data16;
	u8 *Data8;
	
	u32 *srcbuf32;
	u16 *srcbuf16;
	u8 *srcbuf8;

	u32 *dstbuf32;
	u16 *dstbuf16;
	u8 *dstbuf8;
	
	u32 *SrcBuffer32;
	u16 *SrcBuffer16;
	u8 *SrcBuffer8;

	u32 *DstBuffer32;
	u16 *DstBuffer16;
	u8 *DstBuffer8;
	
	u32 Count;
	
	u32 OffsetSrc, OffsetDst;

	u32 ulStartPageSrc, ulStartPageDst;
	u32 ulPageWidthSrc, ulPageHeightSrc;
	u32 ulPageWidthDst, ulPageHeightDst;
	u32 ulBufWidthSrc, ulBufWidthDst;

	u64 *inputdata_ptr;
	
	u32 XferXCount = 0;
	
	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;
	
	// make sure transfer method is set to local->local
	if ( GPURegsGp.TRXDIR.XDIR != 2 )
	{
		cout << "\nhps2x64: ALERT: GPU: Performing local->local transmission while not activated";
		
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
		cout << "\r\nhps2x64: ALERT: GPU: Performing local->local transmission while not activated";
#endif
	}
	
	// get count of pixels to transfer
	Count = XferWidth * XferHeight;
	
	if ( !XferSrcBufWidth )
	{
		if ( !XferX && !XferY )
		{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
	debug << "\r\nGPU: ERROR: Transfer Src Buf Width is ZERO!!!\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Transfer Src Buf Width is ZERO!!!\n";
		}
		
		return;
	}
	
	// check that there is data to transfer
	if ( !XferWidth || !XferHeight ) return;
	
	
#ifdef ALLOW_OPENCL_PS2_TRANSFERIN
	// check for new transfer
	//if ( !XferX && !XferY )
	//{
		//cout << "\nhps2x64: GPU: HW: TRANSFER DATA MOVE!!!*** DPSM=" << hex << GPURegsGp.BITBLTBUF.DPSM << " SPSM=" << GPURegsGp.BITBLTBUF.SPSM;
	//}
#endif


	// if the X specified is greater than buffer width, then modulo
	//if ( XferSrcX >= XferSrcBufWidth ) XferSrcX %= XferSrcBufWidth;
	
	srcbuf32 = & ( RAM32 [ XferSrcOffset32 ] );
	srcbuf16 = (u16*) srcbuf32;
	srcbuf8 = (u8*) srcbuf32;

	dstbuf32 = & ( RAM32 [ XferDstOffset32 ] );
	dstbuf16 = (u16*) dstbuf32;
	dstbuf8 = (u8*) dstbuf32;
	
		if ( ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) < 2 ) && ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) < 2 ) )
		{
			// 32 or 24 bit pixels //
			
			if ( !XferX && !XferY )
			{
				// first transfer for PSMCT32, PSMCT24, PSMZ32, PSMZ24 //
				
				if ( GPURegsGp.BITBLTBUF.SPSM & 1 )
				{
					// PSMCT24, PSMZ24 //
					
					// Limitations for PSMCT24, PSMZ24
					// no limitation on start x-coord
					// width must be a multiple of 8
					if ( XferWidth & 7 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMCT24/PSMZ24 Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMCT24/PSMZ24 Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 7 )
				}
				else
				{
					// PSMCT32, PSMZ32 //
					
					// Limitations for PSMCT32, PSMZ32
					// no limitation on start x-coord
					// width must be a multiple of 2
					if ( XferWidth & 1 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMCT32/PSMZ32 Transfer width not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMCT32/PSMZ32 Transfer width not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 1 )
					
				} // end if ( GPURegsGp.BITBLTBUF.SPSM & 1 )
				
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + XferWidth + ( XferHeight * XferSrcBufWidth ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
				if ( ( ( XferDstOffset32 + XferWidth + ( XferHeight * XferDstBufWidth ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			// 32-bit pixels


#ifdef ALLOW_OPENCL_PS2_TRANSFERMOVE
		// check if using gpu
		if ( bEnable_OpenCL )
		{
			inputdata_ptr = & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];

				// set total pixel count
				//inputdata_ptr [ 11 ] = w * h;



				// new transfer command //

				inputdata_ptr [ 0 ] = GPURegsGp.BITBLTBUF.Value;
				inputdata_ptr [ 1 ] = GPURegsGp.TRXPOS.Value;
				inputdata_ptr [ 2 ] = GPURegsGp.TRXREG.Value;
				inputdata_ptr [ 3 ] = CurrentPath;
				//inputdata_ptr [ 4 ] = WordCount32;
				//inputdata_ptr [ 4 ] = iPixelCount24;
				inputdata_ptr [ 5 ] = XferSrcX;
				inputdata_ptr [ 6 ] = XferSrcY;
				inputdata_ptr [ 7 ] = XferDstX;
				inputdata_ptr [ 8 ] = XferDstY;
				inputdata_ptr [ 9 ] = XferWidth;
				inputdata_ptr [ 10 ] = XferHeight;
				inputdata_ptr [ 11 ] = XferDstBufWidth;
				inputdata_ptr [ 12 ] = XferDstOffset32;
				inputdata_ptr [ 13 ] = XferSrcBufWidth;
				inputdata_ptr [ 14 ] = XferSrcOffset32;

				// transfer in command
				inputdata_ptr [ 15 ] = 0xf1000000ull;


				// new command filled in
				// send the command to the other thread
				// *** todo ***
				ulHwInputBuffer_WriteIndex++;




			// send command
			//Copy_VRAM_toGPU ();
			//FlushToHardware( 0, 1, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );
			//Copy_VRAM_toCPU ();

			//ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
			//ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;

			// should be done
			return;
		}

#endif


			while ( ( XferY < XferHeight ) && Count )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurSrcX &= c_ulTransferXMask;
				CurDstX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;
				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;

				// have to keep pixels in GPU buffer for now
				//SrcBuffer32 = & ( srcbuf32 [ CvtAddrPix32( XferX + XferSrcX, XferY + XferSrcY, XferSrcBufWidth ) ] );
				//DstBuffer32 = & ( dstbuf32 [ CvtAddrPix32( XferX + XferDstX, XferY + XferDstY, XferDstBufWidth ) ] );
				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					case 0:
					case 1:
						//SrcBuffer32 = & ( srcbuf32 [ CvtAddrPix32( XferX + XferSrcX, XferY + XferSrcY, XferSrcBufWidth ) ] );
						SrcBuffer32 = & ( srcbuf32 [ CvtAddrPix32( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					case 0x30:
					case 0x31:
						//SrcBuffer32 = & ( srcbuf32 [ CvtAddrZBuf32( XferX + XferSrcX, XferY + XferSrcY, XferSrcBufWidth ) ] );
						SrcBuffer32 = & ( srcbuf32 [ CvtAddrZBuf32( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
				}
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					case 0:
					case 1:
						//DstBuffer32 = & ( dstbuf32 [ CvtAddrPix32( XferX + XferDstX, XferY + XferDstY, XferDstBufWidth ) ] );
						DstBuffer32 = & ( dstbuf32 [ CvtAddrPix32( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					case 0x30:
					case 0x31:
						//DstBuffer32 = & ( dstbuf32 [ CvtAddrZBuf32( XferX + XferDstX, XferY + XferDstY, XferDstBufWidth ) ] );
						DstBuffer32 = & ( dstbuf32 [ CvtAddrZBuf32( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
				}
			
				if ( ( DstBuffer32 < PtrEnd ) && ( SrcBuffer32 < PtrEnd ) )
				{
					// transfer a pixel
					// *DstBuffer32++ = *SrcBuffer32++;
					// *DstBuffer32 = *SrcBuffer32;
					switch ( GPURegsGp.BITBLTBUF.DPSM & 1 )
					{
						case 0:
						case 0x30:
							// 32-bit pixel //
							*DstBuffer32 = *SrcBuffer32;
							break;
							
						case 1:
						case 0x31:
							// 24-bit pixel //
							*DstBuffer32 = ( ( *SrcBuffer32 ) & 0xffffff ) | ( ( *DstBuffer32 ) & ~0xffffff );
							break;
						
					} // end switch ( GPURegsGp.BITBLTBUF.DPSM & 1 )
				}
				
				
				// update x
				//XferX++;
				XferX += xInc;
				XferXCount++;
				
				// check if greater than width
				if ( XferXCount >= XferWidth )
				{
					// go to next line
					//XferX = 0;
					//XferY++;
					XferX = xStart;
					XferY += yInc;
					
					XferXCount = 0;
					
					// set buffer pointer
					//SrcBuffer32 = & ( srcbuf32 [ ( ( XferX + XferSrcX ) + ( ( XferY + XferSrcY ) * XferSrcBufWidth ) ) ] );
					//DstBuffer32 = & ( dstbuf32 [ ( ( XferX + XferDstX ) + ( ( XferY + XferDstY ) * XferDstBufWidth ) ) ] );
				}
				
				
				// don't go past the number of pixels available to read
				Count--;
			}
		}
		else if ( ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) == 2 ) && ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 2 ) )
		{
			// 16-bit pixels //
			
			// PSMCT16, PSMZ16 //
			
			if ( !XferX && !XferY )
			{
					
					// Limitations for PSMCT16, PSMZ16
					// no limitation on start x-coord
					// width must be a multiple of 4
					if ( XferWidth & 3 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMCT16/PSMZ16 Transfer width not multiple of 4. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMCT16/PSMZ16 Transfer width not multiple of 4. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 3 )
					
					
					
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + ( ( XferWidth + ( XferHeight * XferSrcBufWidth ) ) >> 1 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
				if ( ( ( XferDstOffset32 + ( ( XferWidth + ( XferHeight * XferDstBufWidth ) ) >> 1 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				

				
			}
			
			
			// 16-bit pixels //
			
			while ( ( XferY < XferHeight ) && Count )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurSrcX &= c_ulTransferXMask;
				CurDstX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;
				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;
			
				// have to keep pixels in GPU buffer for now
				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					case 2:
						SrcBuffer16 = & ( srcbuf16 [ CvtAddrPix16 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					case 0xa:
						SrcBuffer16 = & ( srcbuf16 [ CvtAddrPix16S ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					case 0x32:
						SrcBuffer16 = & ( srcbuf16 [ CvtAddrZBuf16 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					case 0x3a:
						SrcBuffer16 = & ( srcbuf16 [ CvtAddrZBuf16S ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
				}
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					case 2:
						DstBuffer16 = & ( dstbuf16 [ CvtAddrPix16 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					case 0xa:
						DstBuffer16 = & ( dstbuf16 [ CvtAddrPix16S ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					case 0x32:
						DstBuffer16 = & ( dstbuf16 [ CvtAddrZBuf16 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					case 0x3a:
						DstBuffer16 = & ( dstbuf16 [ CvtAddrZBuf16S ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
				}
				
				if ( ( DstBuffer16 < PtrEnd ) && ( SrcBuffer16 < PtrEnd ) )
				{
					// transfer a pixel
					// *DstBuffer16++ = *SrcBuffer16++;
					*DstBuffer16 = *SrcBuffer16;
				}
				
				
				// update x
				//XferX++;
				XferX += xInc;
				XferXCount++;
				
				// check if greater than width
				if ( XferXCount >= XferWidth )
				{
					// go to next line
					//XferX = 0;
					//XferY++;
					XferX = xStart;
					XferY += yInc;
					
					XferXCount = 0;
				}
				
				
				// don't go past the number of pixels available to read
				Count--;
			}
		}
		else if ( ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) == 3 ) && ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 3 ) )
		{
			// 8-bit pixels //
			
			// PSMT8, PSMT8H //
			
			if ( !XferX && !XferY )
			{
					
					// Limitations for PSMT8, PSMT8H
					// start x must be a multiple of 2
					if ( XferSrcX & 1 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT8/PSMT8H Transfer SrcX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT8/PSMT8H Transfer SrcX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					}
					if ( XferDstX & 1 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT8/PSMT8H Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT8/PSMT8H Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					}
					
					// width must be a multiple of 8
					if ( XferWidth & 7 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT8/PSMT8H Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT8/PSMT8H Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 7 )
					
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + ( ( XferWidth + ( XferHeight * XferSrcBufWidth ) ) >> 2 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
				if ( ( ( XferDstOffset32 + ( ( XferWidth + ( XferHeight * XferDstBufWidth ) ) >> 2 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				



			}
			
			
			// 8-bit pixels //
			
			while ( ( XferY < XferHeight ) && Count )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurSrcX &= c_ulTransferXMask;
				CurDstX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;
				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;

				// determine format pixel is coming from
				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					// PSMT8
					case 0x13:

						// have to keep pixels in GPU buffer for now
						SrcBuffer8 = & ( srcbuf8 [ CvtAddrPix8( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						
						// get the pixel
						Pixel0 = *SrcBuffer8;
						break;
						
					// PSMT8H
					case 0x1b:
						SrcBuffer32 = & ( srcbuf32 [ CvtAddrPix32( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 = ( *SrcBuffer32 ) >> 24;
						break;
				}
			
				// determine format pixel is going into
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMT8
					case 0x13:
						DstBuffer8 = & ( dstbuf8 [ CvtAddrPix8( CurDstX, CurDstY, XferDstBufWidth ) ] );
						*DstBuffer8 = Pixel0;
						break;
						
					// PSMT8H
					case 0x1b:
						DstBuffer32 = & ( dstbuf32 [ CvtAddrPix32( CurDstX, CurDstY, XferDstBufWidth ) ] );
						*DstBuffer32 = ( ( *DstBuffer32 ) & ~( 0xff << 24 ) ) | ( Pixel0 << 24 );
						break;
				}
					
				
				// update x
				//XferX++;
				XferX += xInc;
				XferXCount++;
				
				// check if greater than width
				// ***TODO*** this would not work
				if ( XferXCount >= XferWidth )
				{
					// go to next line
					//XferX = 0;
					//XferY++;
					XferX = xStart;
					XferY += yInc;
					
					XferXCount = 0;
				}
				
				
				// don't go past the number of pixels available to read
				Count--;
			}
		}
		else if ( ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) == 4 ) && ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 4 ) )
		{
			// 4-bit pixels //
			
			if ( !XferX && !XferY )
			{
					// Limitations for PSMT4, PSMT4HL, PSMT4HH
					// start x must be a multiple of 4
					if ( XferSrcX & 3 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT4/PSMT4HL/PSMT4HH Transfer SrcX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT4/PSMT4HL/PSMT4HH Transfer SrcX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					}
					if ( XferDstX & 3 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT4/PSMT4HL/PSMT4HH Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT4/PSMT4HL/PSMT4HH Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					}
					
					// width must be a multiple of 8
					if ( XferWidth & 7 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT4/PSMT4HL/PSMT4HH Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT4/PSMT4HL/PSMT4HH Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 7 )
					
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + ( ( XferWidth + ( XferHeight * XferSrcBufWidth ) ) >> 3 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
				if ( ( ( XferDstOffset32 + ( ( XferWidth + ( XferHeight * XferDstBufWidth ) ) >> 3 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				



			}
			
			
			
			// 8 times the pixels, but transferring 2 at a time, so times 4
			//WordCount32 <<= 2;
			//Count >>= 1;
			
			while ( ( XferY < XferHeight ) && Count )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurSrcX &= c_ulTransferXMask;
				CurDstX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;
				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;

				// determine format pixel is coming from
				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					// PSMT4
					case 0x14:
						// have to keep pixels in GPU buffer for now
						OffsetSrc = CvtAddrPix4( CurSrcX, CurSrcY, XferSrcBufWidth );
						//OffsetDst = CvtAddrPix4( XferX + XferDstX, XferY + XferDstY, XferDstBufWidth );
						SrcBuffer8 = & ( srcbuf8 [ OffsetSrc >> 1 ] );
						//DstBuffer8 = & ( dstbuf8 [ OffsetDst >> 1 ] );
						
						OffsetSrc = ( OffsetSrc & 1 ) << 2;
						Pixel0 = ( ( *SrcBuffer8 ) >> OffsetSrc ) & 0xf;
						break;
						
					// PSMT4HL
					case 0x24:
						SrcBuffer32 = & ( srcbuf32 [ CvtAddrPix32( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 = ( ( *SrcBuffer32 ) >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						SrcBuffer32 = & ( srcbuf32 [ CvtAddrPix32( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 = ( ( *SrcBuffer32 ) >> 28 ) & 0xf;
						break;
				}
			
				// determine format pixel is going into
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMT4
					case 0x14:
						OffsetDst = CvtAddrPix4( CurDstX, CurDstY, XferDstBufWidth );
						DstBuffer8 = & ( dstbuf8 [ OffsetDst >> 1 ] );
						
						OffsetDst = ( OffsetDst & 1 ) << 2;
						*DstBuffer8 = ( ( *DstBuffer8 ) & ~( 0xf << OffsetDst ) ) | ( Pixel0 << OffsetDst );
						break;
						
					// PSMT4HL
					case 0x24:
						DstBuffer32 = & ( dstbuf32 [ CvtAddrPix32( CurDstX, CurDstY, XferSrcBufWidth ) ] );
						*DstBuffer32 = ( ( *DstBuffer32 ) & ~( 0xf << 24 ) ) | ( Pixel0 << 24 );
						break;
						
					// PSMT4HH
					case 0x2c:
						DstBuffer32 = & ( dstbuf32 [ CvtAddrPix32( CurDstX, CurDstY, XferSrcBufWidth ) ] );
						*DstBuffer32 = ( ( *DstBuffer32 ) & ~( 0xf << 28 ) ) | ( Pixel0 << 28 );
						break;
				}
				
					// update x
					XferX += xInc;
					XferXCount++;
					
					// check if greater than width
					if ( XferXCount >= XferWidth )
					{
						// go to next line
						//XferX = 0;
						//XferY++;
						XferX = xStart;
						XferY += yInc;
						
						XferXCount = 0;
					}
				
				
				// don't go past the number of pixels available to read
				Count--;
			}
		}
		else
		{
			cout << "\nhps2x64: GPU: LOCAL COPY skipped. SPSM=" << hex << GPURegsGp.BITBLTBUF.SPSM << " DPSM=" << GPURegsGp.BITBLTBUF.DPSM;
		}
	//}


}





void GPU::TransferDataOut32_DS ( u32* Data, u32 WordCount32 )
{
#ifdef INLINE_DEBUG_TRANSFER_OUT
	if ( !XferX && !XferY )
	{
		debug << "\r\nTransferOut: ";
		debug << dec << " WC=" << WordCount32;
		debug << dec << " Width=" << XferWidth << " Height=" << XferHeight;
		debug << hex << " SRCPTR32/64=" << GPURegsGp.BITBLTBUF.SBP;
		debug << dec << " DestBufWidth=" << XferSrcBufWidth;
		debug << " InPixFmt=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.SPSM ];
		debug << " OutPixFmt=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.DPSM ];
		debug << " TransferDir=" << TransferDir_Names [ GPURegsGp.TRXPOS.DIR ];
		debug << dec << " XferX=" << XferX << " XferY=" << XferY;
		debug << dec << " XferSrcX=" << XferSrcX << " XferSrcY=" << XferSrcY;
		debug << " @Cycle#" << dec << *_DebugCycleCount;
#ifdef VERBOSE_TRANSFER_OUT
		cout << "\nhps2x64: ALERT: GPU: Transferring data TO memory FROM gpu";
#endif
	}
#endif

#ifdef INLINE_DEBUG_TRANSFER_OUT_2
	debug << "\r\n";
	debug << " XferX=" << dec << XferX << " XferY=" << XferY;
	debug << " WC=" << WordCount32 << " Offset=" << ( ( XferSrcOffset32 + XferX + ( XferY * XferSrcBufWidth ) ) << 2 );
	debug << " SrcPage#" << ( ( XferSrcOffset32 + XferX + ( XferY * XferSrcBufWidth ) ) >> 11 );
	//debug << " XferDstX=" << XferDstX << " XferDstY=" << XferDstY;
#endif

	// all transfer coords wraparound
	const u32 c_ulTransferXMask = 0x7ff;

	u32 CurSrcX, CurSrcY;

	//u64 PixelShift
	u64 PixelLoad;
	u64 Pixel0;
	//u64 PixelCount;
	
	u32 PixelMask;

	// get pointer to dest buffer
	//u32 *DestBuffer;
	
	u32 *Data32;
	u16 *Data16;
	u8 *Data8;
	
	u32 *buf32;
	u16 *buf16;
	u8 *buf8;
	
	u32 *SrcBuffer32;
	u16 *SrcBuffer16;
	u8 *SrcBuffer8;

	u32 *DestBuffer32;
	u16 *DestBuffer16;
	u8 *DestBuffer8;
	
	u32 Offset;
	
	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;

	

	//Finish ();


	
	if ( !XferSrcBufWidth )
	{
		if ( !XferX && !XferY )
		{
#ifdef INLINE_DEBUG_TRANSFER_OUT
	debug << "\r\nGPU: ERROR: Transfer Src Buf Width is ZERO!!!\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Transfer Src Buf Width is ZERO!!!\n";
		}
		
		return;
	}
	
	// make sure transfer method is set to gpu->cpu
	if ( GPURegsGp.TRXDIR.XDIR != 1 )
	{
		cout << "\nhps2x64: ALERT: GPU: Performing gpu->mem transmission while not activated";
		

#ifdef INLINE_DEBUG_TRANSFER_OUT
		debug << "\r\nhps2x64: ALERT: GPU: Performing gpu->mem transmission while not activated";
#endif
	}
	
	// check that there is data to transfer
	if ( !XferWidth || !XferHeight ) return;
	
	
	// if the X specified is greater than buffer width, then modulo
	//if ( XferSrcX >= XferSrcBufWidth ) XferSrcX %= XferSrcBufWidth;
	
	buf32 = & ( RAM32 [ XferSrcOffset32 ] );
	buf16 = (u16*) buf32;
	buf8 = (u8*) buf32;
	
		if ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) < 2 )
		{
			// 32 or 24 bit pixels //
			
			if ( ! ( GPURegsGp.BITBLTBUF.SPSM & 7 ) )
			{
				// 32-bit pixels //
				PixelMask = 0xffffffff;
			}
			else
			{
				// 24-bit pixels //
				PixelMask = 0xffffff;
			}
			
			if ( !XferX && !XferY )
			{
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + XferWidth + ( XferHeight * XferSrcBufWidth ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_OUT
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_OUT
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				

			}
			
			
			// 32-bit pixels
			DestBuffer32 = Data;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurSrcX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;

				// have to keep pixels in GPU buffer for now
				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					// PSMCT32
					case 0:
					// PSMCT24
					case 1:
						SrcBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					// PSMZ32
					case 0x30:
					// PSMZ24
					case 0x31:
						SrcBuffer32 = & ( buf32 [ CvtAddrZBuf32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
				}
			
				if ( SrcBuffer32 < PtrEnd )
				{
					// get the pixel from gpu memory
					Pixel0 = *SrcBuffer32;
					
					switch ( GPURegsGp.BITBLTBUF.SPSM & 1 )
					{
						// PSMCT32
						// PSMZ32
						case 0:
							
							// transfer a pixel
							*DestBuffer32++ = Pixel0;
							WordCount32--;
							break;
							
						// *** TODO: Need to improve the logic here since this might not be bulletproof ***
						// PSMCT24
						// PSMZ24
						case 1:
							
							// mask pixel (only need 24-bits)
							Pixel0 &= 0xffffff;
							
							// pack pixel
							PixelShift |= Pixel0 << PixelCount;
							
							// add the pixel into the count
							PixelCount += 24;
							
							if ( PixelCount >= 32 )
							{
								*DestBuffer32++ = (u32) PixelShift;
								PixelShift >>= 32;
								PixelCount -= 32;
								WordCount32--;
							}
							
							
							break;
					}
					
					
				}
				
				// update x
				XferX++;
				
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
			}
		}
		else if ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) == 2 )
		{
			// 16-bit pixels //
			
			if ( !XferX && !XferY )
			{
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + ( ( XferWidth + ( XferHeight * XferSrcBufWidth ) ) >> 1 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_OUT
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_OUT
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			// 16-bit pixels //
			//SrcBuffer16 = & ( buf16 [ ( ( XferX + XferSrcX ) + ( ( XferY + XferSrcY ) * XferSrcBufWidth ) ) ] );
			DestBuffer16 = (u16*) Data;
			
			// 2 times the pixels
			WordCount32 <<= 1;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurSrcX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;

			
				// have to keep pixels in GPU buffer for now
				//SrcBuffer16 = & ( buf16 [ ( ( XferX + XferSrcX ) + ( ( XferY + XferSrcY ) * XferSrcBufWidth ) ) ] );
				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					// PSMCT16
					case 2:
						SrcBuffer16 = & ( buf16 [ CvtAddrPix16 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					// PSMCT16S
					case 0xa:
						SrcBuffer16 = & ( buf16 [ CvtAddrPix16S ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					// PSMZ16
					case 0x32:
						SrcBuffer16 = & ( buf16 [ CvtAddrZBuf16 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
						
					// PSMZ16S
					case 0x3a:
						SrcBuffer16 = & ( buf16 [ CvtAddrZBuf16S ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						break;
				}
				
				if ( SrcBuffer16 < PtrEnd )
				{
					Pixel0 = *SrcBuffer16++;
					
					// transfer a pixel
					*DestBuffer16++ = Pixel0;
				}
				
				// update x
				XferX++;
				
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;
			}
		}
		else if ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) == 3 )
		{
			// 8-bit pixels //
			
			if ( !XferX && !XferY )
			{
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + ( ( XferWidth + ( XferHeight * XferSrcBufWidth ) ) >> 2 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_OUT
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_OUT
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			// 8-bit pixels //
			//SrcBuffer8 = & ( buf8 [ ( ( XferX + XferSrcX ) + ( ( XferY + XferSrcY ) * XferSrcBufWidth ) ) ] );
			
			DestBuffer8 = (u8*) Data;
			
			// 4 times the pixels
			WordCount32 <<= 2;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurSrcX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;

				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					case 0x13:
						// have to keep pixels in GPU buffer for now
						SrcBuffer8 = & ( buf8 [ CvtAddrPix8 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
					
						// transfer a pixel
						if ( SrcBuffer8 < PtrEnd ) *DestBuffer8++ = *SrcBuffer8++;
						
						break;
						
					case 0x1b:
					
						SrcBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						
						if ( SrcBuffer32 < PtrEnd ) *DestBuffer8++ = ( ( *SrcBuffer32 ) >> 24 );
						break;
				}
				
				// update x
				XferX++;
				
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;
			}
		}
		else if ( ( GPURegsGp.BITBLTBUF.SPSM & 7 ) == 4 )
		{
			// 4-bit pixels //
			
			if ( !XferX && !XferY )
			{
				// check if this transfers outside of GPU memory device
				if ( ( ( XferSrcOffset32 + ( ( XferWidth + ( XferHeight * XferSrcBufWidth ) ) >> 3 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_OUT
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_OUT
					cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			//SrcBuffer8 = & ( buf8 [ ( ( ( XferX + XferSrcX ) >> 1 ) + ( ( XferY + XferSrcY ) * ( XferSrcBufWidth >> 1 ) ) ) ] );
			DestBuffer8 = (u8*) Data;
			
			// 8 times the pixels, but transferring 2 at a time, so times 4
			WordCount32 <<= 2;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurSrcX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;

				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					// PSMT4
					case 0x14:
						// have to keep pixels in GPU buffer for now
						Offset = CvtAddrPix4 ( CurSrcX, CurSrcY, XferSrcBufWidth );
						SrcBuffer8 = & ( buf8 [ Offset >> 1 ] );
			
						// get the first pixel
						Offset = ( Offset & 1 ) << 2;
						Pixel0 = ( ( *SrcBuffer8 ) >> Offset ) & 0xf;
						break;
						
					// PSMT4HL
					case 0x24:
						SrcBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 = ( ( *SrcBuffer32 ) >> 24 ) & 0xf;
						break;
						
					// PSMT4HH
					case 0x2c:
						SrcBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 = ( ( *SrcBuffer32 ) >> 28 ) & 0xf;
						break;
				}
					
					// do the second pixel
					XferX++;
					
					// check if greater than width
					if ( XferX >= XferWidth )
					{
						// go to next line
						XferX = 0;
						XferY++;
					}

				CurSrcX = XferX + XferSrcX;
				CurSrcY = XferY + XferSrcY;

				CurSrcX &= c_ulTransferXMask;
				CurSrcY &= c_ulTransferXMask;

				//if ( CurSrcX >= XferSrcBufWidth ) CurSrcX %= XferSrcBufWidth;

				switch ( GPURegsGp.BITBLTBUF.SPSM )
				{
					// PSMT4
					case 0x14:
						Offset = CvtAddrPix4 ( CurSrcX, CurSrcY, XferSrcBufWidth );
						SrcBuffer8 = & ( buf8 [ Offset >> 1 ] );
						
						// get the second pixel
						Offset = ( Offset & 1 ) << 2;
						Pixel0 |= ( ( ( *SrcBuffer8 ) >> Offset ) & 0xf ) << 4;
						break;
						
					// PSMT4HL
					case 0x24:
						SrcBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 |= ( ( ( *SrcBuffer32 ) >> 24 ) & 0xf ) << 4;
						break;
						
					// PSMT4HH
					case 0x2c:
						SrcBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurSrcX, CurSrcY, XferSrcBufWidth ) ] );
						Pixel0 |= ( ( ( *SrcBuffer32 ) >> 28 ) & 0xf ) << 4;
						break;
				}
					
				// transfer a pixel (ignore checking for end of buffer for now)
				// ***TODO*** check for end of buffer
				*DestBuffer8++ = Pixel0;
				
				// update x
				//XferX++;
				XferX++;
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;
			}
		}
	//}
	
	// check if done
	if ( XferY >= XferHeight )
	{
		// no longer outputting data
		GPU::_GPU->GIFRegs.STAT.OPH = 0;
		
		VU::_VU[ 1 ]->VifRegs.STAT.FDR = 0;
		
		// transfer done
		GPU::_GPU->GIFRegs.STAT.APATH = 0;
		
		// reset XDIR ??
		GPURegsGp.TRXDIR.XDIR = 0;

	}
	
}





void GPU::TransferDataIn32_DS ( u32* Data, u32 WordCount32 )
{
#ifdef INLINE_DEBUG_TRANSFER_IN
	if ( !XferX && !XferY )
	{
		debug << "\r\nTransferIn: ";
		debug << dec << " WC=" << WordCount32;
		debug << dec << " Width=" << XferWidth << " Height=" << XferHeight;
		debug << hex << " DESTPTR32/64=" << GPURegsGp.BITBLTBUF.DBP;
		debug << " InPixFmt=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.SPSM ];
		debug << dec << " DestBufWidth=" << XferDstBufWidth;
		debug << " OutPixFmt=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.DPSM ];
		debug << " TransferDir=" << TransferDir_Names [ GPURegsGp.TRXPOS.DIR ];
		debug << dec << " XferX=" << XferX << " XferY=" << XferY;
		debug << dec << " XferDstX=" << XferDstX << " XferDstY=" << XferDstY;
		debug << " @Cycle#" << dec << *_DebugCycleCount;
		
#ifdef VERBOSE_TRANSFER_IN
		cout << "\nhps2x64: ALERT: GPU: Transferring data TO gpu FROM memory";
#endif
	}
#endif

#ifdef INLINE_DEBUG_TRANSFER_IN_2
	debug << "\r\nTransferIn2: ";
	debug << " XferX=" << dec << XferX << " XferY=" << XferY;
	debug << " WC=" << WordCount32;
	//debug << " Offset=" << ( ( XferDstOffset32 + XferX + ( XferY * XferDstBufWidth ) ) << 2 );
	//debug << " DstPage#" << ( ( XferDstOffset32 + XferX + ( XferY * XferDstBufWidth ) ) >> 11 );
	debug << " DATA=";
	for ( int i = 0; i < WordCount32; i++ )
	{
		debug << hex << " " << Data [ i ];
	}
#endif

	// all transfer coords wraparound
	const u32 c_ulTransferXMask = 0x7ff;

	u32 CurDstX, CurDstY;

	//u64 PixelShift
	u64 PixelLoad;
	u32 Pixel0;
	//u64 PixelCount;

	// get pointer to dest buffer
	//u32 *DestBuffer;
	
	u32 *Data32;
	u16 *Data16;
	u8 *Data8;
	
	u32 *buf32;
	u16 *buf16;
	u8 *buf8;
	
	u32 *DestBuffer32;
	u16 *DestBuffer16;
	u8 *DestBuffer8;
	
	u32 Offset;

	u32 ulStartPage, ulPageWidth, ulPageHeight, ulBufWidth;

	u64 *inputdata_ptr;
	u64 *inputdata_ptr2;
	u32 *inputdata_ptr32;
	int i;
	int iCount;

	s32 iPixelCount24; 

	void *PtrEnd;
	PtrEnd = RAM8 + c_iRAM_Size;
	
	
	// make sure that if multi-threading, that the threads are idle for now (until this part is multi-threaded)
	//Finish ();
	
	
	// make sure transfer method is set to cpu->gpu
	if ( GPURegsGp.TRXDIR.XDIR != 0 )
	{
#ifdef VERBOSE_DEBUG_TRANSFER_IN
		cout << "\nhps2x64: ALERT: GPU: Performing mem->gpu transmission while not activated";
#endif
		
#ifdef INLINE_DEBUG_TRANSFER_IN
		debug << "\r\nhps2x64: ALERT: GPU: Performing mem->gpu transmission while not activated";
#endif

		// since transfer is not activated, must mean to do nothing
		// "disabled" data instead of "image" data
		return;
	}

	
	if ( !XferDstBufWidth )
	{
		if ( !XferX && !XferY )
		{
#ifdef INLINE_DEBUG_TRANSFER_IN
	debug << "\r\nGPU: ERROR: Transfer Dest Buf Width is ZERO!!!\r\n";
#endif

			cout << "\nhps2x64: GPU: ERROR: Transfer Dest Buf Width is ZERO!!!\n";
		}
		
		return;
	}
	
	// check that there is data to transfer
	if ( !XferWidth || !XferHeight ) return;
	

#ifdef ALLOW_OPENCL_PS2_TRANSFERIN
	// check for new transfer
	if ( !XferX && !XferY )
	{
		// if it is a new transfer, give it a new id
		XferId++;
		XferCount32 = 0;

		// reset count of 24-bit pixels
		//iPixelCount24 = 0;

		//if ( GPURegsGp.BITBLTBUF.DPSM > 0xa )
		//{
		//cout << "\nhps2x64: GPU: TransferIn-Compute: Type=" << PixelFormat_Names [ GPURegsGp.BITBLTBUF.DPSM ] << " XferId=" << dec << XferId << " x=" << XferX << " y=" << XferY << " w=" << XferWidth << " h=" << XferHeight;
		//}
	}
#endif
	
	buf32 = & ( RAM32 [ XferDstOffset32 ] );
	buf16 = (u16*) buf32;
	buf8 = (u8*) buf32;
	
	// check if source pixel format is 24-bit
	// this is transferring TO GPU, so must check the destination for the correct value
	if ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 1 )
	{
		// 24-bit pixels //

#ifdef ALLOW_OPENCL_PS2_TRANSFERIN
		// check if using gpu
		if ( bEnable_OpenCL )
		{
			inputdata_ptr = & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];

				// set the id
				//inputdata_ptr [ 14 ] = XferId;

				// set the start index in pixel input buffer (start index from target index)
				inputdata_ptr [ 16 ] = ullPixelInBuffer_WriteIndex - ullPixelInBuffer_TargetIndex;

				// set the count of 32-bit words in block
				//inputdata_ptr [ 4 ] = WordCount32;

				// set total pixel count
				//inputdata_ptr [ 11 ] = w * h;

			// copy pixels into buffer
			iPixelCount24 = 0;
			//for ( i = 0; i < WordCount32; i++ )
			while( WordCount32 || ( PixelCount >= 3 ) )
			{
				if ( PixelCount < 3 )
				{
					// load next data
					PixelLoad = *Data++;
					
					WordCount32--;
					
					// put into pixel
					PixelShift |= ( PixelLoad << ( PixelCount << 3 ) );
					
					PixelCount += 4;
				}

				ulPixelInBuffer32 [ ullPixelInBuffer_WriteIndex & c_ullPixelInBuffer_Mask ] = PixelShift;
				ullPixelInBuffer_WriteIndex++;

				// count the transferred pixels
				iPixelCount24++;

				// shift (24-bit pixels)
				PixelShift >>= 24;
				
				// update pixel count
				PixelCount -= 3;
			}





			//if ( WordCount32 > 32 )
			//{
			//	cout << "\nhps2x64: ALERT: WordCount32=" << dec << WordCount32;
			//}

#ifdef ENABLE_HWPIXEL_INPUT

			// check that previous entry
			inputdata_ptr2 = & pHwInputBuffer64 [ ( (ulHwInputBuffer_WriteIndex-1) & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];

			// check if part of previous command or new command
			if (
				( ulHwInputBuffer_TargetIndex > (ulHwInputBuffer_WriteIndex-1) )
				|| ( inputdata_ptr2 [ 14 ] != XferId )
				|| ( inputdata_ptr2 [ 15 ] != ( 0xf8ull << 24 ) )
			)
			{
				// new transfer command //

				inputdata_ptr [ 0 ] = GPURegsGp.BITBLTBUF.Value;
				inputdata_ptr [ 1 ] = GPURegsGp.TRXPOS.Value;
				inputdata_ptr [ 2 ] = GPURegsGp.TRXREG.Value;
				inputdata_ptr [ 3 ] = CurrentPath;
				//inputdata_ptr [ 4 ] = WordCount32;
				inputdata_ptr [ 4 ] = iPixelCount24;
				inputdata_ptr [ 5 ] = XferX;
				inputdata_ptr [ 6 ] = XferY;
				inputdata_ptr [ 7 ] = XferDstX;
				inputdata_ptr [ 8 ] = XferDstY;
				inputdata_ptr [ 9 ] = XferWidth;
				inputdata_ptr [ 10 ] = XferHeight;
				inputdata_ptr [ 11 ] = XferDstBufWidth;
				inputdata_ptr [ 12 ] = XferDstOffset32;

				// put in the start count of 32-bit units
				// ***todo*** if using the count for gpu transfers, then need to conver XferX,XferY before switching to cpu tranfer
				inputdata_ptr [ 13 ] = XferCount32;

				// need to know if transfer is related
				inputdata_ptr [ 14 ] = XferId;

				// transfer in command
				inputdata_ptr [ 15 ] = 0xf8000000ull;


				// new command filled in
				// send the command to the other thread
				// *** todo ***
				ulHwInputBuffer_WriteIndex++;
			}
			else
			{
				// same transfer command //

				// update the count of pixels in previous command
				//inputdata_ptr2 [ 4 ] += WordCount32;
				inputdata_ptr2 [ 4 ] += iPixelCount24;
			}



			// update count of 32-bit data transferred
			//XferCount32 += WordCount32;
			XferCount32 += iPixelCount24;

			// this is just to keep transfer from resetting for now
			// can update XferX,XferY with the real values when switching between software/hardware
			//XferX += WordCount32;
			XferX += iPixelCount24;

			// send command
			//Copy_VRAM_toGPU ();
			//FlushToHardware( 0, 1, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );
			//Copy_VRAM_toCPU ();

			//ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
			//ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;

			// should be done
			return;
#endif
		}

#endif


		if ( !XferX && !XferY )
		{
			// first transfer for PSMCT24, PSMZ24 //
				
			// PSMCT24, PSMZ24 //

#ifdef ALLOW_OPENCL_PS2_TRANSFERIN
			cout << "\nhps2x64: ALERT: GPU: SHADER: 24-bit transfers not support by shader renderer yet!";
#endif
			// Limitations for PSMCT24, PSMZ24
			// no limitation on start x-coord
			// width must be a multiple of 8
			if ( XferWidth & 7 )
			{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
				debug << "\r\n***PSMCT24/PSMZ24 Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
				cout << "\nhps2x64: ALERT: GPU: PSMCT24/PSMZ24 Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif
			} // end if ( XferWidth & 7 )
			
					
				
			// check if this transfers outside of GPU memory device
			if ( ( ( XferDstOffset32 + XferWidth + ( XferHeight * XferDstBufWidth ) ) << 2 ) > c_iRAM_Size )
			{
#ifdef INLINE_DEBUG_TRANSFER_IN
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_IN
				cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
			}
			
			
		}
		
		
		//DestBuffer32 = & ( buf32 [ ( ( XferX + XferDstX ) + ( ( XferY + XferDstY ) * XferDstBufWidth ) ) ] );
		Data32 = Data;
		
		
		while ( ( XferY < XferHeight ) && ( WordCount32 || ( PixelCount >= 3 ) ) )
		{
			CurDstX = XferX + XferDstX;
			CurDstY = XferY + XferDstY;

			CurDstX &= c_ulTransferXMask;
			CurDstY &= c_ulTransferXMask;

			//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;

			// check if you need to load
			if ( PixelCount < 3 )
			{
				// load next data
				PixelLoad = *Data32++;
				
				WordCount32--;
				
				// put into pixel
				PixelShift |= ( PixelLoad << ( PixelCount << 3 ) );
				
				PixelCount += 4;
			}
				
			// get pixel
			Pixel0 = PixelShift & 0xffffff;
			
			// have to keep pixels in GPU buffer for now
			//DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( XferX + XferDstX, XferY + XferDstY, XferDstBufWidth ) ] );
			switch ( GPURegsGp.BITBLTBUF.DPSM )
			{
				// PSMCT24
				case 1:
					DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
					break;
					
				// PSMZ24
				case 0x31:
					DestBuffer32 = & ( buf32 [ CvtAddrZBuf32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
					break;
			}
			
			if ( DestBuffer32 < PtrEnd )
			{
			
				// transfer pixel
				// *DestBuffer32++ = ( Pixel0 & 0xffffff ) | ( ( *DestBuffer32 ) & ~0xffffff );
				*DestBuffer32 = ( Pixel0 & 0xffffff ) | ( ( *DestBuffer32 ) & ~0xffffff );
			}
			
			// shift (24-bit pixels)
			PixelShift >>= 24;
			
			// update pixel count
			PixelCount -= 3;
			
			// update x
			XferX++;
			
			// check if greater than width
			if ( XferX >= XferWidth )
			{
				// go to next line
				XferX = 0;
				XferY++;
				
			}
			
		}
	}
	else
	{
		// not 24-bit pixels //

#ifdef ALLOW_OPENCL_PS2_TRANSFERIN
		// check if using gpu
		if ( bEnable_OpenCL )
		{
			inputdata_ptr = & pHwInputBuffer64 [ ( ulHwInputBuffer_WriteIndex & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];

			// fill in data structure for gpu //
			// inputbuffer
			// 0: BITBLTBUF
			// 1: TRXPOS
			// 2: TRXREG
			// 3: CurrentPath
			// 4: Count64
			// 5: XferX
			// 6: XferY
			// 7: XferDstX
			// 8: XferDstY
			// 9: XferWidth
			// 10: XferHeight
			// 11: XferDstBufWidth
			// 12: XferDstOffset32
			// 13: 
			// 14: ADDITIONAL COMMAND
			// -------------
			// 15: PRIM (COMMAND)
			// 16: Data0
			// ...
			// 31: Data15

			inputdata_ptr [ 0 ] = GPURegsGp.BITBLTBUF.Value;
			inputdata_ptr [ 1 ] = GPURegsGp.TRXPOS.Value;
			inputdata_ptr [ 2 ] = GPURegsGp.TRXREG.Value;
			inputdata_ptr [ 3 ] = CurrentPath;
			inputdata_ptr [ 4 ] = WordCount32;
			inputdata_ptr [ 5 ] = XferX;
			inputdata_ptr [ 6 ] = XferY;
			inputdata_ptr [ 7 ] = XferDstX;
			inputdata_ptr [ 8 ] = XferDstY;
			inputdata_ptr [ 9 ] = XferWidth;
			inputdata_ptr [ 10 ] = XferHeight;
			inputdata_ptr [ 11 ] = XferDstBufWidth;
			inputdata_ptr [ 12 ] = XferDstOffset32;

			// put in the start count of 32-bit units
			// ***todo*** if using the count for gpu transfers, then need to conver XferX,XferY before switching to cpu tranfer
			inputdata_ptr [ 13 ] = XferCount32;

			// need to know if transfer is related
			inputdata_ptr [ 14 ] = XferId;

			// transfer in command
			inputdata_ptr [ 15 ] = 0xf8000000ull;

			//if ( WordCount32 > 32 )
			//{
			//	cout << "\nhps2x64: ALERT: WordCount32=" << dec << WordCount32;
			//}

#ifdef ENABLE_HWPIXEL_INPUT

			// check that previous entry
			inputdata_ptr2 = & pHwInputBuffer64 [ ( (ulHwInputBuffer_WriteIndex-1) & c_ulHwInputBuffer_Mask ) << c_ulHwInputBuffer_Shift ];

			// check if part of previous command or new command
			if (
				( ulHwInputBuffer_TargetIndex > (ulHwInputBuffer_WriteIndex-1) )
				|| ( inputdata_ptr2 [ 14 ] != XferId )
				|| ( inputdata_ptr2 [ 15 ] != ( 0xf8ull << 24 ) )
			)
			{
				// new transfer command //

				// set the id
				inputdata_ptr [ 14 ] = XferId;

				// set the start index in pixel input buffer (start index from target index)
				inputdata_ptr [ 16 ] = ullPixelInBuffer_WriteIndex - ullPixelInBuffer_TargetIndex;

				// set the count of 32-bit words in block
				inputdata_ptr [ 4 ] = WordCount32;

				// set total pixel count
				//inputdata_ptr [ 11 ] = w * h;


				// new command filled in
				// send the command to the other thread
				// *** todo ***
				ulHwInputBuffer_WriteIndex++;
			}
			else
			{
				// same transfer command //

				// update the count of pixels in previous command
				inputdata_ptr2 [ 4 ] += WordCount32;
			}


			// copy pixels into buffer
			for ( i = 0; i < WordCount32; i++ )
			{
				ulPixelInBuffer32 [ ullPixelInBuffer_WriteIndex & c_ullPixelInBuffer_Mask ] = Data [ i ];
				ullPixelInBuffer_WriteIndex++;
			}

			/*
			if ( ( GPURegsGp.BITBLTBUF.DPSM == 0x13 ) && ( XferCount32 < 4 ) )
			{
				// dispatch our test command
				FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

				ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
				ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

				ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
				ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;

			// get result
			Copy_VRAM_toCPU ();
			debug << "\r\nTRANSFERINHW";
			debug << " t0=" << hex << RAM16[1000000+0] << " " << RAM16[1000000+1] << " " << RAM16[1000000+2];
			debug << " t1=" << hex << RAM16[1000000+4] << " " << RAM16[1000000+5] << " " << RAM16[1000000+6];
			debug << " t2=" << hex << RAM16[1000000+8] << " " << RAM16[1000000+9] << " " << RAM16[1000000+10];
			debug << " t3=" << hex << RAM16[1000000+12] << " " << RAM16[1000000+13] << " " << RAM16[1000000+14];

			debug << " l0=" << hex << (LUT_XOFFSET[ (12 + XferDstX) | (0x9 << 12) ] ^ LUT_YOFFSET[ (15 + XferDstY) | (0x9 << 7) ]);
			debug << " l1=" << hex << (LUT_XOFFSET[ (13 + XferDstX) | (0x9 << 12) ] ^ LUT_YOFFSET[ (15 + XferDstY) | (0x9 << 7) ]);
			debug << " l2=" << hex << (LUT_XOFFSET[ (14 + XferDstX) | (0x9 << 12) ] ^ LUT_YOFFSET[ (15 + XferDstY) | (0x9 << 7) ]);
			debug << " l3=" << hex << (LUT_XOFFSET[ (15 + XferDstX) | (0x9 << 12) ] ^ LUT_YOFFSET[ (15 + XferDstY) | (0x9 << 7) ]);

			debug << " c0=" << hex << CvtAddrPix8 ( 12 + XferDstX, 15 + XferDstY, XferDstBufWidth );
			debug << " c1=" << hex << CvtAddrPix8 ( 13 + XferDstX, 15 + XferDstY, XferDstBufWidth );
			debug << " c2=" << hex << CvtAddrPix8 ( 14 + XferDstX, 15 + XferDstY, XferDstBufWidth );
			debug << " c3=" << hex << CvtAddrPix8 ( 15 + XferDstX, 15 + XferDstY, XferDstBufWidth );
			}
			*/

			// update count of 32-bit data transferred
			XferCount32 += WordCount32;

			// this is just to keep transfer from resetting for now
			// can update XferX,XferY with the real values when switching between software/hardware
			XferX += WordCount32;



			// should be done
			return;

#endif	// end #ifdef ENABLE_HWPIXEL_INPUT
		}

/*
#else

			FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

			ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
			ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

			ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
			ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;


			// get result
			Copy_VRAM_toCPU ();
			*/


#endif	// end #ifdef ALLOW_OPENCL_PS2_TRANSFERIN

		
		if ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 0 )
		{
			// 32-bit pixels //
			
			if ( !XferX && !XferY )
			{
				// PSMCT32, PSMZ32 //
				
				// Limitations for PSMCT32, PSMZ32
				// no limitation on start x-coord
				// width must be a multiple of 2
				if ( XferWidth & 1 )
				{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
					debug << "\r\n***PSMCT32/PSMZ32 Transfer width not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
					cout << "\nhps2x64: ALERT: GPU: PSMCT32/PSMZ32 Transfer width not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
				} // end if ( XferWidth & 1 )
				
				// check if this transfers outside of GPU memory device
				if ( ( ( XferDstOffset32 + XferWidth + ( XferHeight * XferDstBufWidth ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_IN
				debug << "\r\n***PSMCT32 Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_IN
				cout << "\nPSMCT32 Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}

			// 32-bit pixels
			//DestBuffer32 = & ( buf32 [ ( ( XferX + XferDstX ) + ( ( XferY + XferDstY ) * XferDstBufWidth ) ) /* & ( c_iRAM_Mask >> 2 ) */ ] );
			Data32 = Data;

			
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurDstX &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;

				// have to keep pixels in GPU buffer for now
				//DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( XferX + XferDstX, XferY + XferDstY, XferDstBufWidth ) ] );
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMCT32
					case 0:
						DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					// PSMZ32
					case 0x30:
						DestBuffer32 = & ( buf32 [ CvtAddrZBuf32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
				}
			
				if ( DestBuffer32 < PtrEnd )
				{
					Pixel0 = *Data32++;
					

					// transfer a pixel
					*DestBuffer32++ = Pixel0;
				}
				
				// update x
				XferX++;
				
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;
			}
		}
		else if ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 2 )
		{
			// 16-bit pixels //
			
			if ( !XferX && !XferY )
			{
					// Limitations for PSMCT16, PSMZ16
					// no limitation on start x-coord
					// width must be a multiple of 4
					if ( XferWidth & 3 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMCT16/PSMZ16 Transfer width not multiple of 4. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMCT16/PSMZ16 Transfer width not multiple of 4. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 3 )
					
				// check if this transfers outside of GPU memory device
				if ( ( ( XferDstOffset32 + ( ( XferWidth + ( XferHeight * XferDstBufWidth ) ) >> 1 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_IN
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_IN
				cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			// 16-bit pixels //
			//DestBuffer16 = & ( buf16 [ ( ( XferX + XferDstX ) + ( ( XferY + XferDstY ) * XferDstBufWidth ) ) ] );
			Data16 = (u16*) Data;


			// 2 times the pixels
			WordCount32 <<= 1;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurDstX &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;
			
				// have to keep pixels in GPU buffer for now
				//DestBuffer16 = & ( buf16 [ ( ( XferX + XferDstX ) + ( ( XferY + XferDstY ) * XferDstBufWidth ) ) ] );
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMCT16
					case 2:
						DestBuffer16 = & ( buf16 [ CvtAddrPix16 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
					
					// PSMCT16S
					case 0xa:
						DestBuffer16 = & ( buf16 [ CvtAddrPix16S ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					// PSMZ16
					case 0x32:
						DestBuffer16 = & ( buf16 [ CvtAddrZBuf16 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
						
					// PSMZ16S
					case 0x3a:
						DestBuffer16 = & ( buf16 [ CvtAddrZBuf16S ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						break;
				}
				
				if ( DestBuffer16 < PtrEnd )
				{
					Pixel0 = *Data16++;
					

					// transfer a pixel
					*DestBuffer16++ = Pixel0;
				}
				
				// update x
				XferX++;
				
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;
			}
		}
		else if ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 3 )
		{
			// 8-bits per pixel //
			
			if ( !XferX && !XferY )
			{
					// Limitations for PSMT8, PSMT8H
					// start x must be a multiple of 2
					if ( XferDstX & 1 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT8/PSMT8H Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT8/PSMT8H Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					}
					
					// width must be a multiple of 8
					if ( XferWidth & 7 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT8/PSMT8H Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT8/PSMT8H Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 7 )
					
				// check if this transfers outside of GPU memory device
				//if ( ( ( XferDstOffset32 + ( ( XferWidth + ( XferHeight * XferDstBufWidth ) ) >> 2 ) ) << 2 ) > c_iRAM_Size )
				if ( ( ( XferDstOffset32 << 2 ) + ( XferWidth + ( XferHeight * XferDstBufWidth ) ) ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_IN
				debug << "\r\n***PSMT8 Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
				debug << "\r\nXferDstOffset8=" << dec << ( XferDstOffset32 << 2 ) << " DataSize=" << (XferWidth * XferHeight) << " RAMSize=" << c_iRAM_Size;
#endif

#ifdef VERBOSE_TRANSFER_IN
				cout << "\nPSMT8 Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			// 8-bit pixels //
			//DestBuffer8 = & ( buf8 [ ( ( XferX + XferDstX ) + ( ( XferY + XferDstY ) * XferDstBufWidth ) ) ] );
			Data8 = (u8*) Data;
			
			// 4 times the pixels
			WordCount32 <<= 2;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurDstX &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;

				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMT8
					case 0x13:
						// have to keep pixels in GPU buffer for now
						DestBuffer8 = & ( buf8 [ CvtAddrPix8 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						
						// transfer a pixel
						if ( DestBuffer8 < PtrEnd )
						{
							*DestBuffer8 = *Data8++;
						}
						else
						{
							cout << "\nhps2x64: transfer outside of GPU RAM\n";
							// DestBuffer8 -= c_iRAM_Size;
							// *DestBuffer8 = *Data8++;
						}
						break;
						
					// PSMT8H
					case 0x1b:
						DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						
						PixelLoad = *DestBuffer32;
						PixelLoad = ( PixelLoad & ~( 0xff << 24 ) ) | ( ( (u32) ( *Data8 ) ) << 24 );
						
						// transfer a pixel
						if ( DestBuffer32 < PtrEnd ) *DestBuffer32 = PixelLoad;
						
						// go to next source pixel
						Data8++;
						
						break;
				}
			
				
				// update x
				XferX++;
				
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;
			}
		}
		else if ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 4 )
		{
			// 4-bits per pixel //
			
			if ( !XferX && !XferY )
			{
					// Limitations for PSMT4, PSMT4HL, PSMT4HH
					// start x must be a multiple of 4
					if ( XferDstX & 3 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT4/PSMT4HL/PSMT4HH Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT4/PSMT4HL/PSMT4HH Transfer DstX not multiple of 2. Cycle#" << dec << *_DebugCycleCount;
#endif
					}
					
					// width must be a multiple of 8
					if ( XferWidth & 7 )
					{
#ifdef INLINE_DEBUG_TRANSFER_LOCAL_LIMIT
						debug << "\r\n***PSMT4/PSMT4HL/PSMT4HH Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_LOCAL_LIMIT
						cout << "\nhps2x64: ALERT: GPU: PSMT4/PSMT4HL/PSMT4HH Transfer width not multiple of 8. Cycle#" << dec << *_DebugCycleCount;
#endif
					} // end if ( XferWidth & 7 )
					
				// check if this transfers outside of GPU memory device
				if ( ( ( XferDstOffset32 + ( ( XferWidth + ( XferHeight * XferDstBufWidth ) ) >> 3 ) ) << 2 ) > c_iRAM_Size )
				{
#ifdef INLINE_DEBUG_TRANSFER_IN
				debug << "\r\n***Transfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif

#ifdef VERBOSE_TRANSFER_IN
				cout << "\nTransfer will extend outside GPU RAM. Cycle#" << dec << *_DebugCycleCount;
#endif
				}
				
			}
			
			
			// 4-bit pixels //
			//DestBuffer8 = & ( buf8 [ ( ( ( XferX + XferDstX ) >> 1 ) + ( ( XferY + XferDstY ) * ( XferDstBufWidth >> 1 ) ) ) ] );
			Data8 = (u8*) Data;
			
			// 8 times the pixels, but transferring 2 at a time, so times 4
			WordCount32 <<= 2;
			
			while ( ( XferY < XferHeight ) && WordCount32 )
			{
				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurDstX &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;
				
				//if ( DestBuffer8 < PtrEnd )
				//{
					// get the first pixel
					PixelShift = ( *Data8 ) & 0xf;

				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMT4
					case 0x14:
						// have to keep pixels in GPU buffer for now
						Offset = CvtAddrPix4 ( CurDstX, CurDstY, XferDstBufWidth );
						DestBuffer8 = & ( buf8 [ Offset >> 1 ] );
						
						Offset = ( Offset & 1 ) << 2;
						PixelLoad = *DestBuffer8;
						PixelLoad = ( PixelLoad & ~( 0xf << Offset ) ) | ( PixelShift << Offset );
						
						// transfer a pixel
						// *DestBuffer8++ = *Data8++;
						if ( DestBuffer8 < PtrEnd ) *DestBuffer8 = PixelLoad;
						break;
						
					// PSMT4HL
					case 0x24:
						DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						
						PixelLoad = *DestBuffer32;
						PixelLoad = ( PixelLoad & ~( 0xf << 24 ) ) | ( PixelShift << 24 );
						if ( DestBuffer32 < PtrEnd ) *DestBuffer32 = PixelLoad;
						break;
						
					// PSMT4HH
					case 0x2c:
						DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						
						PixelLoad = *DestBuffer32;
						PixelLoad = ( PixelLoad & ~( 0xf << 28 ) ) | ( PixelShift << 28 );
						if ( DestBuffer32 < PtrEnd ) *DestBuffer32 = PixelLoad;
						break;
				}
					
					
					// update x
					XferX++;


				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
					// set buffer pointer
					//DestBuffer8 = & ( buf8 [ ( ( ( XferX + XferDstX ) >> 1 ) + ( ( XferY + XferDstY ) * ( XferDstBufWidth >> 1 ) ) ) ] );
				}


				CurDstX = XferX + XferDstX;
				CurDstY = XferY + XferDstY;

				CurDstX &= c_ulTransferXMask;
				CurDstY &= c_ulTransferXMask;

				//if ( CurDstX >= XferDstBufWidth ) CurDstX %= XferDstBufWidth;


					// get the second pixel
					PixelShift = ( ( *Data8 ) >> 4 ) & 0xf;
					
					// do the second pixel
				switch ( GPURegsGp.BITBLTBUF.DPSM )
				{
					// PSMT4
					case 0x14:
						// have to keep pixels in GPU buffer for now
						Offset = CvtAddrPix4 ( CurDstX, CurDstY, XferDstBufWidth );
						DestBuffer8 = & ( buf8 [ Offset >> 1 ] );
						
						Offset = ( Offset & 1 ) << 2;
						PixelLoad = *DestBuffer8;
						PixelLoad = ( PixelLoad & ~( 0xf << Offset ) ) | ( PixelShift << Offset );
						
						// transfer a pixel
						// *DestBuffer8++ = *Data8++;
						if ( DestBuffer8 < PtrEnd ) *DestBuffer8 = PixelLoad;
						break;
						
					// PSMT4HL
					case 0x24:
						DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						
						PixelLoad = *DestBuffer32;
						PixelLoad = ( PixelLoad & ~( 0xf << 24 ) ) | ( PixelShift << 24 );
						if ( DestBuffer32 < PtrEnd ) *DestBuffer32 = PixelLoad;
						break;
						
					// PSMT4HH
					case 0x2c:
						DestBuffer32 = & ( buf32 [ CvtAddrPix32 ( CurDstX, CurDstY, XferDstBufWidth ) ] );
						
						PixelLoad = *DestBuffer32;
						PixelLoad = ( PixelLoad & ~( 0xf << 28 ) ) | ( PixelShift << 28 );
						if ( DestBuffer32 < PtrEnd ) *DestBuffer32 = PixelLoad;
						break;

				}	// end switch ( GPURegsGp.BITBLTBUF.DPSM )
					
					// update source pointer to next byte
					Data8++;
				//}
				
				// update x
				//XferX++;
				XferX++;
				
				// check if greater than width
				if ( XferX >= XferWidth )
				{
					// go to next line
					XferX = 0;
					XferY++;
					
				}
				
				
				// don't go past the number of pixels available to read
				WordCount32--;

			}	// end while ( ( XferY < XferHeight ) && WordCount32 )

		}	// end else if ( ( GPURegsGp.BITBLTBUF.DPSM & 7 ) == 4 )

//#ifndef ALLOW_OPENCL_PS2_TRANSFERIN
//
//			Copy_VRAM_toGPU ();
//
//#endif

	}



}



void GPU::InitCvtLUTs ()
{
	u32 x, y, dx, dy;
	u32 Offset;
	
	for ( dx = 0; dx < 128; dx++ )
	{
		for ( dy = 0; dy < 128; dy++ )
		{
			x = dx & 0x3f;
			y = dy & 0x1f;
			Offset = ( x & 0x1 ) | ( ( ( y & 0x1 ) | ( x & 0x6 ) ) << 1 ) | ( ( ( y & 0x6 ) | ( x & 0x8 ) ) << 3 ) | ( ( ( y & 0x8 ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 5 );
			LUT_CvtAddrPix32 [ x + ( y << 6 ) ] = Offset;

			// note: can shift offset as needed for shader
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x00 << 7 ) | dy ] = Offset;
				LUT_YOFFSET [ ( 0x0d << 7 ) | dy ] = Offset;
				LUT_YOFFSET [ ( 0x12 << 7 ) | dy ] = Offset;
				LUT_YOFFSET [ ( 0x16 << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x00 << 14 ) | ( dy << 7 ) | dx ] = Offset;
			LUT_XYOFFSET [ ( 0x0d << 14 ) | ( dy << 7 ) | dx ] = Offset;
			LUT_XYOFFSET [ ( 0x12 << 14 ) | ( dy << 7 ) | dx ] = Offset;
			LUT_XYOFFSET [ ( 0x16 << 14 ) | ( dy << 7 ) | dx ] = Offset;
			
			Offset = ( x & 0x1 ) | ( ( ( y & 0x1 ) | ( x & 0x6 ) ) << 1 ) | ( ( ( y & 0x6 ) | ( x & 0x8 ) ) << 3 ) | ( ( ( y & 0x8 ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 5 );
			Offset ^= 0x600;
			LUT_CvtAddrZBuf32 [ x + ( y << 6 ) ] = Offset;

			//LUT_OFFSET [ 0x18 | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x18 << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x18 << 14 ) | ( dy << 7 ) | dx ] = Offset;

			x = dx & 0x3f;
			y = dy & 0x3f;
			Offset = ( ( x & 8 ) >> 3 ) | ( ( x & 1 ) << 1 ) | ( ( ( y & 1 ) | ( x & 6 ) ) << 2 ) | ( ( ( y & 0xe ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 5 ) | ( ( y & 0x20 ) << 6 );
			LUT_CvtAddrPix16 [ x + ( y << 6 ) ] = Offset;

			//LUT_OFFSET [ 0x01 | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x01 << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x01 << 14 ) | ( dy << 7 ) | dx ] = Offset;
			
			Offset = ( ( x & 8 ) >> 3 ) | ( ( x & 1 ) << 1 ) | ( ( ( y & 1 ) | ( x & 6 ) ) << 2 ) | ( ( ( y & 0x2e ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 6 );
			LUT_CvtAddrPix16s [ x + ( y << 6 ) ] = Offset;

			//LUT_OFFSET [ 0x05 | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x05 << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x05 << 14 ) | ( dy << 7 ) | dx ] = Offset;

			Offset = ( ( x & 8 ) >> 3 ) | ( ( x & 1 ) << 1 ) | ( ( ( y & 1 ) | ( x & 6 ) ) << 2 ) | ( ( ( y & 0xe ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 5 ) | ( ( y & 0x20 ) << 6 );
			Offset ^= 0xc00;
			LUT_CvtAddrZBuf16 [ x + ( y << 6 ) ] = Offset;

			//LUT_OFFSET [ 0x19 | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x19 << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x19 << 14 ) | ( dy << 7 ) | dx ] = Offset;
			
			Offset = ( ( x & 8 ) >> 3 ) | ( ( x & 1 ) << 1 ) | ( ( ( y & 1 ) | ( x & 6 ) ) << 2 ) | ( ( ( y & 0x2e ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 6 );
			Offset ^= 0xc00;
			LUT_CvtAddrZBuf16s [ x + ( y << 6 ) ] = Offset;

			//LUT_OFFSET [ 0x1d | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x1d << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x1d << 14 ) | ( dy << 7 ) | dx ] = Offset;
			
			x = dx;
			y = dy & 0x3f;
			Offset = ( ( y & 2 ) >> 1 ) | ( ( x & 8 ) >> 2 ) | ( ( x & 1 ) << 2 ) | ( ( ( y & 1 ) | ( x & 6 ) ) << 3 ) | ( ( ( y & 0xc ) | ( x & 0x10 ) ) << 4 ) | ( ( ( y & 0x10 ) | ( x & 0x20 ) ) << 5 ) | ( ( ( y & 0x20 ) | ( x & 0x40 ) ) << 6 );
			Offset ^= ( ( y & 2 ) << 4 ) ^ ( ( y & 4 ) << 3 );
			LUT_CvtAddrPix8 [ x + ( y << 7 ) ] = Offset;

			//LUT_OFFSET [ 0x09 | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x09 << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x09 << 14 ) | ( dy << 7 ) | dx ] = Offset;
			
			x = dx;
			y = dy;
			Offset = ( ( y & 2 ) >> 1 ) | ( ( x & 0x18 ) >> 2 ) | ( ( x & 1 ) << 3 ) | ( ( ( y & 1 ) | ( x & 6 ) ) << 4 ) | ( ( ( y & 0x1c ) | ( x & 0x20 ) ) << 5 ) | ( ( ( y & 0x20 ) | ( x & 0x40 ) ) << 6 ) | ( ( y & 0x40 ) << 7 );
			Offset ^= ( ( y & 2 ) << 5 ) ^ ( ( y & 4 ) << 4 );
			LUT_CvtAddrPix4 [ x + ( y << 7 ) ] = Offset;

			//LUT_OFFSET [ 0x0a | ( ( dx & 0x7f ) << 5 ) | ( ( dy & 0x7f ) << ( 5 + 7 ) ) ] = Offset;
			if ( !dx )
			{
				LUT_YOFFSET [ ( 0x0a << 7 ) | dy ] = Offset;
			}
			LUT_XYOFFSET [ ( 0x0a << 14 ) | ( dy << 7 ) | dx ] = Offset;
		}
	}
	
	//static u32 LUT_CvtAddrPix32 [ 64 * 32 ];
	//static u32 LUT_CvtAddrPix16 [ 64 * 64 ];
	//static u32 LUT_CvtAddrPix16s [ 64 * 64 ];
	//static u32 LUT_CvtAddrPix8 [ 128 * 64 ];
	//static u32 LUT_CvtAddrPix4 [ 128 * 128 ];
	
	//static u32 LUT_CvtAddrZBuf32 [ 64 * 32 ];
	//static u32 LUT_CvtAddrZBuf16 [ 64 * 64 ];
	//static u32 LUT_CvtAddrZBuf16s [ 64 * 64 ];

}



void GPU::OutputTo_P1Buffer ( u64 *pMemDevice, u32 Address, u64 CycleCount )
{
#ifdef INLINE_DEBUG_P1BUFFER
	debug << "\r\n\r\nGPU::OutputTo_P1Buffer; " << hex << setw ( 8 ) << *_DebugPC << " " << dec << *_DebugCycleCount << " Address= " << hex << Address << "; CycleCount= " << dec << CycleCount;
	debug << " WI(before)=" << ullP1Buf_WriteIndex;
	debug << " RI=" << ullP1Buf_ReadIndex;
#endif

	GPU::GIFTag0_t gt0;
	GPU::GIFTag1_t gt1;
	
	u64 ullTWriteIndex;
	
	u64 ullStartIndex;
	
	u64 ullTotalCount;
	
	u64 ullRegCount;
	u64 ullDataCount;
	
	
	// get the write index
	ullTWriteIndex = ullP1Buf_WriteIndex;
	
	// get the starting point
	ullStartIndex = ullTWriteIndex;
	
	// write the cycle count and number of 128-bit chunks
	ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 0 ] = CycleCount;
	//ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 1 ] = ullDataCount;
	ullTWriteIndex++;
	

	do
	{
		// get the gif tags
		//gt0.Value = *pData++;
		//gt1.Value = *pData++;
		gt0.Value = pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 0 ];
		gt1.Value = pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 1 ];
		
#ifdef INLINE_DEBUG_P1BUFFER
	debug << " ADDR=" << hex << Address;
	debug << " gt0=" << gt0.Value;
	debug << " gt1=" << gt1.Value;
#endif

		// write the gif tags
		ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 0 ] = gt0.Value;
		ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 1 ] = gt1.Value;
		ullTWriteIndex++;
		Address++;
		

#ifdef VERBOSE_PATH1_NOTDATA128
		if ( gt0.FLG )
		{
			// was assuming 128-bit data from path 1 //
			cout << "\nhps2x64: GPU: PATH1: data in path1 is not 128-bit commands. FLG=" << gt0.FLG;
		}
#endif

		if ( gt0.FLG == 1 )
		{
			// get the amount of data to transfer (regs*loop)
			ullRegCount = ( ( !gt0.REGS ) ? ( 16 ) : ( gt0.REGS ) );
			ullDataCount = ullRegCount * gt0.NLOOP;

			//if ( ullDataCount & 1 )
			//{
			//	cout << "\nhps2x64: GPU: PATH1: data in path1 is not 128-bit aligned. FLG=1";
			//}

			// align to 128-bits for now in p1 buffer
			ullDataCount = ( ullDataCount + 1 ) >> 1;

			for ( u64 ullIdx = 0; ullIdx < ullDataCount; ullIdx++ )
			{
				ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 0 ] = pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 0 ];
				ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 1 ] = pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 1 ];
				ullTWriteIndex++;
				Address++;

#ifdef DISABLE_PATH1_WRAP
				if ( Address >= 0x400 ) break;
#endif
			}
		}
		else
		{
			// get the amount of data to transfer (regs*loop)
			ullRegCount = ( ( !gt0.REGS ) ? ( 16 ) : ( gt0.REGS ) );
			ullDataCount = ullRegCount * gt0.NLOOP;

#ifdef INLINE_DEBUG_P1BUFFER
	debug << " Cnt=" << ullDataCount;
#endif

			// write the data packet
			for ( u64 ullIdx = 0; ullIdx < ullDataCount; ullIdx++ )
			{
#ifdef INLINE_DEBUG_P1BUFFER
	debug << " ADDR=" << hex << Address;
	debug << " Data=" << hex << pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 0 ] << " " << pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 1 ];
#endif

				//ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 0 ] = *pData++;
				//ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 1 ] = *pData++;
				ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 0 ] = pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 0 ];
				ullPath1Buffer [ ( ( ullTWriteIndex & c_iPath1BufMask ) << 1 ) + 1 ] = pMemDevice [ ( ( Address & 0x3ff ) << 1 ) + 1 ];
				ullTWriteIndex++;
				Address++;
			
#ifdef DISABLE_PATH1_WRAP
				if ( Address >= 0x400 ) break;
#endif
			}

		}
		
		
	// loop until end of transfer is reached
	} while ( ( !gt0.EOP )
#ifdef DISABLE_PATH1_WRAP
		&& ( Address < 0x400 )
#endif
	);
	
	// get the total count of 128-bit chunks
	// need to do minus one because the count includes the first 128-bit block
	ullTotalCount = ullTWriteIndex - ullStartIndex - 1;
	
	// write the total count of 128-bit chunks in transfer
	ullPath1Buffer [ ( ( ullStartIndex & c_iPath1BufMask ) << 1 ) + 1 ] = ullTotalCount;
	
	// write back the write index
	ullP1Buf_WriteIndex = ullTWriteIndex;

	if ( ( ullP1Buf_WriteIndex - ullP1Buf_ReadIndex ) > c_iPath1BufSize )
	{
		cout << "\nhps2x64: GPU: ALERT: P1 buffer overrun! READIDX=" << ullP1Buf_ReadIndex << " WRITEIDX=" << ullP1Buf_WriteIndex;
	}


//#ifdef ENABLE_P1MT
	// for now, other threads should stop reading here too
	//ullP1Buf_TargetIndex = ullP1Buf_WriteIndex;
//#endif
	
#ifdef INLINE_DEBUG_P1BUFFER
	debug << " WI(after)=" << dec << ullP1Buf_WriteIndex;
#endif
}



void GPU::Flush ()
{
	// flush the p1 buffer of data if it is time for the data to be flushed
	//GPU::Path1_ProcessBlocks ();

	//cout << "\nGPU::FLUSH ulNumberOfThreads_Created=" << dec << ulNumberOfThreads_Created << " ulInputBuffer_WriteIndex=" << ulInputBuffer_WriteIndex << " ulInputBuffer_TargetIndex=" << ulInputBuffer_TargetIndex;

	//if ( _GPU->ulNumberOfThreads || Playstation1::SPU2::_SPU2->ulNumThreads )
	if ( ulNumberOfThreads_Created )
	{
		//cout << "\nGPU::Flush ulInputBuffer_WriteIndex" << ulInputBuffer_WriteIndex << " ulInputBuffer_TargetIndex=" << ulInputBuffer_TargetIndex;
		if ( ( ulInputBuffer_WriteIndex != ulInputBuffer_TargetIndex )
			|| ( _GPU->ullP1Buf_WriteIndex != _GPU->ullP1Buf_TargetIndex )
		)
		{
			//cout << "\nGPU FLUSH";

			//ulInputBuffer_TargetIndex = ulInputBuffer_WriteIndex;
			
			// send the command to the other thread
			//_GPU->ullP1Buf_TargetIndex = _GPU->ullP1Buf_WriteIndex;
			x64ThreadSafe::Utilities::Lock_Exchange64 ( (long long&) _GPU->ullP1Buf_TargetIndex, _GPU->ullP1Buf_WriteIndex );
			x64ThreadSafe::Utilities::Lock_Exchange64 ( (long long&) ulInputBuffer_TargetIndex, ulInputBuffer_WriteIndex );
			
			// trigger event
			if ( !SetEvent ( ghEvent_PS2GPU_Update ) )
			{
				cout << "\nUnable to set PS2 GPU UPDATE event. " << GetLastError ();
			}
			
			//while ( ( ulInputBuffer_WriteIndex - x64ThreadSafe::Utilities::Lock_ExchangeAdd64 ( (long long&) ulInputBuffer_ReadIndex, 0 ) ) > ( c_ulInputBuffer_Size - c_ulRequiredBuffer ) );
//cout << "\nSENDING: " << dec << ulInputBuffer_WriteIndex;
		}
	}
	else
	{
		if ( _GPU->bEnable_OpenCL )
		{
			if ( ulHwInputBuffer_WriteIndex != ulHwInputBuffer_ReadIndex )
			{

				FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

				ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
				ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

				ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
				ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;
			}
		}	// end if ( bEnable_OpenCL )

	}
}

void GPU::Finish ()
{
	// flush the p1 buffer of data if it is time for the data to be flushed
	//GPU::Path1_ProcessBlocks ();

	//if ( _GPU->ulNumberOfThreads || Playstation1::SPU2::_SPU2->ulNumThreads )
	if ( ulNumberOfThreads_Created )
	{
		//if ( ulInputBuffer_WriteIndex != x64ThreadSafe::Utilities::Lock_ExchangeAdd64 ( (long long&)ulInputBuffer_ReadIndex, 0 ) )
		if ( ( ulInputBuffer_WriteIndex != ulInputBuffer_ReadIndex )
			|| ( _GPU->ullP1Buf_WriteIndex != _GPU->ullP1Buf_ReadIndex )
		)
		{
			
			//cout << "\nGPU FINISH";

			if ( ( ulInputBuffer_WriteIndex != ulInputBuffer_TargetIndex )
				|| ( _GPU->ullP1Buf_WriteIndex != _GPU->ullP1Buf_TargetIndex )
			)
			{
				//ulInputBuffer_TargetIndex = ulInputBuffer_WriteIndex;
				
				// send the command to the other thread
				//_GPU->ullP1Buf_TargetIndex = _GPU->ullP1Buf_WriteIndex;
				x64ThreadSafe::Utilities::Lock_Exchange64 ( (long long&) _GPU->ullP1Buf_TargetIndex, _GPU->ullP1Buf_WriteIndex );
				x64ThreadSafe::Utilities::Lock_Exchange64 ( (long long&) ulInputBuffer_TargetIndex, ulInputBuffer_WriteIndex );
				
				if ( !ResetEvent ( ghEvent_PS2GPU_Finish ) )
				{
					cout << "\nUnable to reset finish event before update. " << GetLastError ();
				}
				
				// trigger event
				if ( !SetEvent ( ghEvent_PS2GPU_Update ) )
				{
					cout << "\nUnable to set PS2 GPU UPDATE event. " << GetLastError ();
				}
			}
			
			// wait for the other thread to complete
			//while ( ulInputBuffer_WriteIndex != x64ThreadSafe::Utilities::Lock_ExchangeAdd64 ( (long long&)ulInputBuffer_ReadIndex, 0 ) )
			//{
#ifdef ENABLE_WAIT_DURING_SPIN_FINISH
		//cout << "\nWaiting for finish...";
				//MsgWaitForMultipleObjectsEx( NULL, NULL, 1, QS_ALLINPUT, MWMO_ALERTABLE );
				//SetEvent ( ghEvent_PS2GPU_Update );
				WaitForSingleObject ( ghEvent_PS2GPU_Finish, INFINITE );
		//cout << "\nDONE Waiting for finish...";
#endif

				//PAUSE ();
			//}
		}
	}
	else
	{
		if ( _GPU->bEnable_OpenCL )
		{
			if ( ulHwInputBuffer_WriteIndex != ulHwInputBuffer_ReadIndex )
			{

				FlushToHardware ( ulHwInputBuffer_ReadIndex, ulHwInputBuffer_WriteIndex, ullPixelInBuffer_ReadIndex, ullPixelInBuffer_WriteIndex );

				ulHwInputBuffer_TargetIndex = ulHwInputBuffer_WriteIndex;
				ulHwInputBuffer_ReadIndex = ulHwInputBuffer_WriteIndex;

				ullPixelInBuffer_ReadIndex = ullPixelInBuffer_WriteIndex;
				ullPixelInBuffer_TargetIndex = ullPixelInBuffer_WriteIndex;
			}
		}	// end if ( bEnable_OpenCL )

	}
}



