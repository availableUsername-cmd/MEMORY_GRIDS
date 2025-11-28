#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <windows.h>

#define LAYOUTS_NUMBER 1U

#define TEXT2_NUMBER 2U

#define MAX_ID 1024U
#define MAX_SIZE 6U

#define START_X 0U
#define START_Y -289U

typedef enum{
EMPTY = 0U,
FOOD = 1U,
WALL = 2U,
SPIKE = 3U,
TREE = 4U,
SLEEPER = 5U,
INFORMATION = 6U,
THINKER = 7U,
RICH = 8U,
HUMOR = 9U,
EATER = 10U,
MOVER = 11U,
PRODUCER = 12U
} CellType ;

typedef struct{
    int x;
    int y;
} Vec2;

typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
} AppState;

typedef struct{
    _Float16 x, y;
    _Float16 w, h;
} Rect_Ratio;

typedef struct{
    int height;
    int width;
    char* context;
} Matrix;

Vec2 vec2_add(Vec2 a,Vec2 b){
    return (Vec2){a.x+b.x,a.y+b.y};
}


void SDL_RenderClearBlack(SDL_Renderer*renderer){
    SDL_SetRenderDrawColor(renderer,0,0,64,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void PaintGrids(int size,int x,int y,int w,int h,SDL_Renderer* renderer,SDL_Texture* texture,const Matrix* matrix){
    SDL_SetRenderTarget(renderer,texture);
    SDL_RenderClearBlack(renderer);
    int count=0;
    for (int i=y; i < h+y ; i++)
    {
        for (int j = x; j < w+x; j++)
        {
            SDL_FRect r={(j-x)*size,(i-y)*size,size,size};
            //count++;
            switch (matrix->context[j+i*matrix->width])
            {
            case EMPTY:
                //黑
                SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case FOOD:
                //棕,ADD
                SDL_SetRenderDrawColor(renderer,128,64,0,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case SPIKE:
                //红
                SDL_SetRenderDrawColor(renderer,255,0,0,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case TREE:
                //深绿
                SDL_SetRenderDrawColor(renderer,0,192,0,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case SLEEPER:
                //紫色
                SDL_SetRenderDrawColor(renderer,128,0,64,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case INFORMATION:
                //白
                SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case THINKER:
                //蓝
                SDL_SetRenderDrawColor(renderer,0,0,255,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case WALL:
                //灰
                SDL_SetRenderDrawColor(renderer,128,128,128,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case EATER:
                //黄
                SDL_SetRenderDrawColor(renderer,255,255,0,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case MOVER:
                //青色
                SDL_SetRenderDrawColor(renderer,0,255,255,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            case PRODUCER:
                //绿色
                SDL_SetRenderDrawColor(renderer,0,255,0,SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer,&r);
                break;
            default:
                break;
            }
            //SDL_SetRenderDrawColor(renderer,192,192,192,SDL_ALPHA_OPAQUE);
            //SDL_RenderRect(renderer,&r);
            
        }
    }
    SDL_SetRenderTarget(renderer,NULL);
    //SDL_Log("count:%d,w*h%d",count,w*h);
}

void MatrixInit(Matrix* matrix,int height,int width){
    matrix->context=SDL_malloc(height*width*sizeof(char));

    matrix->height=height;
    matrix->width=width;
}

void MatrixFree(Matrix* matrix){
    SDL_free(matrix->context);
    matrix->height=0;
    matrix->width=0;
}

BOOL CALLBACK 藏侄流程(HWND hwnd,LPARAM lparam){
    HWND brother = FindWindowEx(hwnd,0,"SHELLDLL_DefView",0);
    if ( brother != 0){
        HWND nephew=FindWindowEx(0,hwnd,"WorkerW",0);
        ShowWindow(nephew,SW_HIDE);
        return FALSE;
    }
    return TRUE;
}

void 认爹流程(HWND window){
    HWND desktop=FindWindow("Progman", NULL);

    SendMessage(desktop,0x52C,0,0);

    HWND hwnd = FindWindowEx(NULL, NULL, "WorkerW", NULL);

    /* 快速鉴定：检查有没有侄子
    if (FindWindowEx(hwnd, NULL, NULL, NULL)) {
        // 如果这个有侄子 → 可能是图标管家
        // 找下一个
       hwnd = FindWindowEx(NULL, hwnd, "WorkerW", NULL); 
    } */

    EnumWindows(藏侄流程,0);

    //if (hwnd) ShowWindow(hwnd, SW_HIDE);

    SetParent(window, desktop); //正式认爹

    //SetWindowPos(window,HWND_BOTTOM,0,0,0,0,SWP_NOSIZE); //蹲在最底层
}

int main(int argc,char *argv[]){
    
    AppState as;
    SDL_Event operation_event;

    bool quit=0;
    bool focused=1;

    int x_loop_count=0;
    bool loop_have_no_event=true;
    int cycletime=0;

    int grid_x= START_X;
    int grid_y= START_Y;
    int block_size_in_pixel=64U;

    //初始化SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    //以及ttf
    if (!TTF_Init()) {
        return SDL_APP_FAILURE;
    }

    //应用SDL功能应该在初始化之后！！！

    //初始化矩阵！（使用了SDL_malloc）
    Matrix matrix;
    MatrixInit(&matrix,128U,128U);

    //获取显示屏
    SDL_DisplayID DI = SDL_GetPrimaryDisplay();
    SDL_DisplayMode const *DM = SDL_GetDesktopDisplayMode(DI);
    int window_width=DM->w;int window_height=DM->h;
    SDL_Log("w:%d,h:%d",DM->w,DM->h);

    //初始化窗口及渲染器
    if (!SDL_CreateWindowAndRenderer( "网格", window_width, window_height, SDL_WINDOW_BORDERLESS, &as.window, &as.renderer)) {
        return SDL_APP_FAILURE;
    }
    

    //获取窗口句柄并作为桌面
    SDL_PropertiesID props=SDL_GetWindowProperties(as.window);
    HWND hwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    认爹流程(hwnd);


    //准备纹理，可以在不清空的情况下存储图像
    SDL_Surface *surface;
    SDL_Texture *texture_grids =SDL_CreateTexture(as.renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,DM->w,DM->h);

    //初始化各布局，使用比例以保证在不同显示器适配
    //这些没用玩意不要了！！！！

    //下面用于渲染UI布局边框，Render渲染器先把UI贴到texture中，以避免后面的渲染器会覆盖前面的!#注1：辣鸡代码全丢了我不要啦！！

    //下面用来渲染网格
    int w=window_width/block_size_in_pixel+1;int h=window_height/block_size_in_pixel+1;
    SDL_Log("gw:%d,gh:%d",w,h);
    PaintGrids(block_size_in_pixel,grid_x,grid_y,w,h,as.renderer,texture_grids,&matrix);

    SDL_Log("枚举占%d字节",sizeof(CellType));

    //主循环
    while ( !quit )
    {
        /*if (!focused) {
            SDL_WaitEvent(NULL);              // 暂停直到有新事件                      
        }*/

        //判断事件
        //哈哈！其实几乎没有用！这是壁纸永远没焦点的啊哈哈哈哈哈！！！
        while (SDL_PollEvent(&operation_event))
        {
            switch (operation_event.type)
            {
            
            case SDL_EVENT_QUIT:
                quit^=1;
                SDL_Log("Quit");
                break;
            
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                SDL_Log("Focus Losted");
                focused=0;
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                SDL_Log("Focus gained");
                focused=1;
                break;
            
            case SDL_EVENT_MOUSE_WHEEL:
                if ((operation_event.wheel.y >0) && window_height>block_size_in_pixel*2)
                {
                    block_size_in_pixel*=2;
                } else if ((operation_event.wheel.y <0) && window_width<block_size_in_pixel/2*matrix.width)
                {
                    block_size_in_pixel/=2;
                } else break;
                //啊啊啊啊气死我了不能在里面加int啊啊啊啊啊啊要不然只要在switch里有效了啊啊啊啊啊！！！！！！😵😵🤪🤪😡😡😭😭😱😱
                w=window_width/block_size_in_pixel+1;
                h=window_height/block_size_in_pixel+1;
                SDL_Log("gw:%d,gh:%d",w,h);
                SDL_Log("Scale Factor:%.2f",(float)block_size_in_pixel/32U);
                PaintGrids(block_size_in_pixel,grid_x,grid_y,w,h,as.renderer,texture_grids,&matrix);
                break;
                
            case SDL_EVENT_KEY_DOWN:
                SDL_SetRenderTarget(as.renderer,texture_grids);
                SDL_RenderClear(as.renderer);
                loop_have_no_event=0;
                switch (operation_event.key.scancode)
                {
                
                case SDL_SCANCODE_ESCAPE:
                    SDL_Log("esc_pressed");
                    quit^=1;
                    break;

                case SDL_SCANCODE_UP:
                    grid_y-=1;
                    break;
                case SDL_SCANCODE_DOWN:
                    grid_y+=1;
                    break;
                case SDL_SCANCODE_LEFT:
                    grid_x-=1;
                    break;
                case SDL_SCANCODE_RIGHT:
                    grid_x+=1;
                    break;

                default:
                    break;
                }
                break;
            default:

                break;
            }
        }

        PaintGrids(block_size_in_pixel,grid_x,grid_y,w,h,as.renderer,texture_grids,&matrix);
        SDL_RenderClearBlack(as.renderer);

        SDL_RenderTexture(as.renderer,texture_grids,NULL,NULL);

        SDL_RenderPresent(as.renderer);
        SDL_Delay(50);

        //经验证，这些数字在我的电脑上不会导致越界，你的电脑就不知道了
        if (loop_have_no_event) {grid_x+=1;x_loop_count+=1;}
        if (x_loop_count>=768) {grid_y+=19; SDL_Log("Now Start Point:%d",grid_x+grid_y*matrix.width);cycletime+=1;x_loop_count=0;}
        if (cycletime>15) {grid_x=START_X;grid_y=START_Y;cycletime=0;}
        loop_have_no_event=1;
        //SDL_Log("eoml");
    }

    SDL_DestroyRenderer(as.renderer);
    SDL_DestroyWindow(as.window);
    SDL_Quit();


}
