#include "ArmedSprite.h"
#include "DDManager.h"
#include "DIManager.h"
#include "DXAManager.h"
#include "FXManager.h"
#include "GameManager.h"
#include "Menu.h"
#include "Sprites.h"
#include "Starfield.h"
#include <Windows.h>
#include <stdio.h>

short FrameRate;

#define PLAYER_MAX_HEALTH 4
#define PLAYER_MOVE_DIST 700
#define MAX_HISCORE_ENTRYS 10
#define MULTIPLAYER_START_TIME 180

HWND hWnd = nullptr;

DirectDrawManager DDM;
DirectXAudioManager DMM;
FXManager FXM;

static BOOL Running = TRUE, InGame = FALSE, QuitQuery = FALSE,
            PlayerDieSequence = FALSE;

static LPDIRECTDRAWSURFACE7 lpDDSLogo;

int Error(const char* string)
{
  MessageBox(hWnd, string, "Fehler", MB_OK | MB_ICONEXCLAMATION);
  OutputDebugString(string);
  OutputDebugString("\n");

  return FALSE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_KEYDOWN:
      switch (wParam)
      {
        case VK_ESCAPE:
          if (Running && InGame)
            QuitQuery = TRUE;
          // DestroyWindow(hWnd);
          break;

        case 0x50: // VK_P
        case VK_PAUSE:
          if (!InGame || PlayerDieSequence)
            break;

          Running = !Running;

          RECT r = {0, 0, 500, 200};
          DDM.GetBuffer()->BltFast(
            150, 200, lpDDSLogo, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

          DDM.Flip();
          DDM.CopyPrimaryToBackBuffer();
          break;
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  return 0;
}

HWND CreateMainWindow(HINSTANCE hInstance)
{
  WNDCLASSEX wndClass;

  wndClass.cbSize = sizeof(WNDCLASSEX);
  wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = WindowProc;
  wndClass.cbClsExtra = 0;
  wndClass.cbWndExtra = 0;
  wndClass.hInstance = hInstance;
  wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndClass.hCursor = nullptr;
  wndClass.lpszMenuName = nullptr;
  wndClass.lpszClassName = "MyWndClass";
  wndClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
  wndClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

  RegisterClassEx(&wndClass);

  return CreateWindowEx(
    NULL,
    "MyWndClass",
    "Starfield Battle",
    WS_POPUP | WS_VISIBLE,
    0,
    0,
    0,
    0,
    nullptr,
    nullptr,
    hInstance,
    nullptr);
}

void ShowText(
  UINT Alignment,
  COLORREF Color,
  int x,
  int y,
  LPCTSTR Text,
  LPDIRECTDRAWSURFACE7 lpDDS)
{
  HDC hDC;

  lpDDS->GetDC(&hDC);

  SetTextColor(hDC, Color);
  SetBkMode(hDC, TRANSPARENT);
  SetTextAlign(hDC, Alignment);

  TextOut(hDC, x, y, Text, lstrlen(Text));

  lpDDS->ReleaseDC(hDC);
}

static int Scan2Ascii(DWORD Scancode, USHORT* Result)
{
  static HKL Layout = GetKeyboardLayout(0);
  static UCHAR State[256];

  if (!GetKeyboardState(State))
    return 0;

  UINT Vk = MapVirtualKeyEx(Scancode, 1, Layout);

  return ToAsciiEx(Vk, Scancode, State, Result, 0, Layout);
}

BOOL DoMainInits()
{
  SetCursor(nullptr);

  if (!DDM.Init(hWnd))
    return Error("Fehler beim Initialisieren von DirectDraw");

  if (!DMM.Init(hWnd, "Sounds"))
    return Error("Fehler beim Initialisieren von DirectX Audio");

  DMM.SetGlobalVolume();

  if (!DMM.AddSndFX("PlShoot.wav"))
    return FALSE;
  if (!DMM.AddSndFX("Enemy1.wav"))
    return FALSE;
  if (!DMM.AddSndFX("Enemy2.wav"))
    return FALSE;
  if (!DMM.AddSndFX("Enemy3.wav"))
    return FALSE;
  if (!DMM.AddSndFX("Explosion.wav"))
    return FALSE;
  // if (!DMM.AddSndFX("Brrzz.wav"))     return FALSE;
  if (!DMM.AddBGMusic("Music.mid"))
    return FALSE;

  FXM.Init(12, DDM.GetBuffer());

  return TRUE;
}

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  hWnd = CreateMainWindow(hInstance);

  if (nullptr == hWnd)
  {
    Error("Fenster konnte nicht erzeugt werden!");
    return -1;
  }

  if (!DoMainInits())
    return -1;

  LPDIRECTDRAWSURFACE7 lpDDSBackBuffer = DDM.GetBuffer();

  SpriteManager Enemys;
  Enemys.Init(5, lpDDSBackBuffer);

  DirectInputManager DIM;

  if (!DIM.Init(hWnd, hInstance))
  {
    Error("Fehler beim Initialisiern von DirectInput");
    return -1;
  }

  LPDIRECTDRAWSURFACE7 lpDDSPlayer =
    DDM.CreateSurfaceFromBitmap("Sprites\\Player.bmp", 150, 120);
  LPDIRECTDRAWSURFACE7 lpDDSPlayer2 =
    DDM.CreateSurfaceFromBitmap("Sprites\\Player2.bmp", 150, 120);
  LPDIRECTDRAWSURFACE7 lpDDSPlShot =
    DDM.CreateSurfaceFromBitmap("Sprites\\PlShoot.bmp", 60, 20);
  LPDIRECTDRAWSURFACE7 lpDDSPlShot2 =
    DDM.CreateSurfaceFromBitmap("Sprites\\PlShoot2.bmp", 60, 20);
  lpDDSLogo = DDM.CreateSurfaceFromBitmap("Logos.bmp", 500, 665);
  LPDIRECTDRAWSURFACE7 lpDDSMenu =
    DDM.CreateSurfaceFromBitmap("Menu.bmp", 460, 800);
  // LPDIRECTDRAWSURFACE7 lpDDSPlMuzzle =
  // DDM.CreateSurfaceFromBitmap("Sprites\\Player_Muzzle.bmp", 56, 26);

  RECT rItem = {0, 0, 460, 100};
  POINT p = {175, 50};
  GameMenu Menu(lpDDSMenu, 4, rItem, p);

  DDCOLORKEY ddck;
  ddck.dwColorSpaceHighValue = 0;
  ddck.dwColorSpaceLowValue = 0;

  lpDDSLogo->SetColorKey(DDCKEY_SRCBLT, &ddck);
  // lpDDSPlMuzzle->SetColorKey(DDCKEY_SRCBLT, &ddck);

  Starfield Stars(150, lpDDSBackBuffer);

  DDM.ClearScreen(0);
  Stars.Draw(TRUE);
  RECT r = {0, 200, 500, 400};
  lpDDSBackBuffer->BltFast(
    150, 200, lpDDSLogo, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
  DDM.Flip();
  DDM.CopyPrimaryToBackBuffer();

  LONGLONG Frequency = 0, CurrentTime = 0, LastTime = 0;

  short FrameCount = 0;

  if (!QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency))
  {
    Error("Performance-Counter nicht vorhanden!");
    return -1;
  }

  QueryPerformanceCounter((LARGE_INTEGER*)&LastTime);

  for (;;)
  {
    QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

    if (CurrentTime - LastTime > Frequency) // eine Sekunde ist vergangen
    {
      FrameRate = FrameCount;
      FrameCount = 0;
      break;
    }
    else
      FrameCount++;

    DDM.Flip();
  }

  Sleep(1000);

  // ------------------------------------------------------------------------------------------

  ArmedSprite Player, Player2;
  Player.Create(lpDDSPlayer, 75, 120, MSECS(80), TRUE);
  Player.SetShotData(
    lpDDSPlShot, 12, 20, MSECS(16), TRUE, 3, -600, "PlShoot.wav", 25);
  // Player.SetMuzzleFlash(lpDDSPlMuzzle);
  Player.SetNewBBox(8, 28, 65, 84);

  Player2.Create(lpDDSPlayer2, 75, 120, MSECS(80), TRUE);
  Player2.SetShotData(
    lpDDSPlShot2, 12, 20, MSECS(16), TRUE, 3, -600, "PlShoot.wav", 25);
  Player2.SetNewBBox(8, 28, 65, 84);

  BOOL GameOver = FALSE, MultiPlayer = FALSE;

  int PlHealth = PLAYER_MAX_HEALTH, PlNewDmgDelay = 0;
  int PlPoints = 0, Pl2Pts = 0;

  int MultiPlayerTime = MULTIPLAYER_START_TIME;

  const POINT PlHlthPos = {0, 600 - 65};

  RECT PlFrmSize = {0, 600, 55, 665};

  KeyActionMap KeyMap = {DIK_LEFT, DIK_RIGHT, DIK_UP, DIK_DOWN, DIK_SPACE};
  KeyActionMap Pl2_KeyMap = {DIK_A, DIK_D, DIK_W, DIK_S, DIK_TAB};

  HiScoreEntry Entrys[MAX_HISCORE_ENTRYS];
  FILE* fScoreList = fopen("Scores.dat", "rb");

  if (nullptr == fScoreList)
  {
    int tmp = 10 * MAX_HISCORE_ENTRYS;

    for (auto& Entry : Entrys)
    {
      strcpy(Entry.Name, "BFGBoom Productions");
      Entry.Score = tmp;
      tmp -= 10;
    }
  }
  else
  {
    short Nr = fread(
      (void*)Entrys, sizeof(HiScoreEntry), MAX_HISCORE_ENTRYS, fScoreList);

    if (Nr < MAX_HISCORE_ENTRYS)
    {
      for (int i = Nr; i < MAX_HISCORE_ENTRYS; i++)
      {
        strcpy(Entrys[i].Name, "BFGBoom Productions");
        Entrys[i].Score = 100 - 10 * i;
      }
    }

    fclose(fScoreList);
  }

  int LowestScore = Entrys[MAX_HISCORE_ENTRYS - 1].Score;

  BOOL ViewScores = FALSE;

  DMM.PlayBGMusic();

  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------

  MSG msg;

  ZeroMemory(&msg, sizeof(MSG));

  BOOL NewSecond = FALSE;
  BOOL MarkPlacing = FALSE;
  int Placing = 0;

  SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

  QueryPerformanceCounter((LARGE_INTEGER*)&LastTime);

  while (msg.message != WM_QUIT)
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

      if (CurrentTime - LastTime > Frequency)
      {
        NewSecond = TRUE;
        LastTime = CurrentTime;
        FrameRate = FrameCount;
        FrameCount = 0;
      }
      else
      {
        FrameCount++;
        NewSecond = FALSE;
      }

      if (Running || !InGame)
      {
        DDM.ClearScreen(0);
        Stars.Draw();
      }

      if (InGame)
      {
        /* ----------> */ if (Running && !PlayerDieSequence)
        {
          if (QuitQuery)
          {
            QuitQuery = FALSE;
            GameOver = PlayerDieSequence = TRUE;
          }

          Player.Draw(lpDDSBackBuffer);
          Player.DrawShots(lpDDSBackBuffer);

          int PlAction = DIM.GetInput(KeyMap);

          if (PlAction & MOVE_LEFT)
            Player.Move(float(-PLAYER_MOVE_DIST) / FrameRate, 0);

          if (PlAction & MOVE_RIGHT)
            Player.Move(float(PLAYER_MOVE_DIST) / FrameRate, 0);

          if (PlAction & MOVE_UP)
            Player.Move(0, float(-PLAYER_MOVE_DIST) / FrameRate);

          if (PlAction & MOVE_DOWN)
            Player.Move(0, float(PLAYER_MOVE_DIST) / FrameRate);

          if (PlAction & FIRE)
            Player.Fire();

          if (MultiPlayer)
          {
            Player2.Draw(lpDDSBackBuffer);
            Player2.DrawShots(lpDDSBackBuffer);

            int Pl2Action = DIM.GetInput(Pl2_KeyMap);

            if (Pl2Action & MOVE_LEFT)
              Player2.Move(float(-PLAYER_MOVE_DIST) / FrameRate, 0);

            if (Pl2Action & MOVE_RIGHT)
              Player2.Move(float(PLAYER_MOVE_DIST) / FrameRate, 0);

            if (Pl2Action & MOVE_UP)
              Player2.Move(0, float(-PLAYER_MOVE_DIST) / FrameRate);

            if (Pl2Action & MOVE_DOWN)
              Player2.Move(0, float(PLAYER_MOVE_DIST) / FrameRate);

            if (Pl2Action & FIRE)
              Player2.Fire();
          }

          Enemys.MoveAndShoot();
          FXM.Draw();

          int i = 0;

          if (!MultiPlayer)
          {
            for (; i < PlHealth - 1; i++)
            {
              lpDDSBackBuffer->BltFast(
                PlHlthPos.x + 65 * i,
                PlHlthPos.y,
                lpDDSLogo,
                &PlFrmSize,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            }

            char str[18];
            sprintf(str, "Score: %d", PlPoints);
            ShowText(TA_LEFT, RGB(255, 255, 255), 0, 0, str, lpDDSBackBuffer);
          }
          else
          {
            char Str1[18], Str2[18];
            sprintf(Str1, "Player1: %d points", PlPoints);
            sprintf(Str2, "Player2: %d points", Pl2Pts);
            ShowText(TA_LEFT, RGB(255, 255, 255), 0, 0, Str1, lpDDSBackBuffer);
            ShowText(TA_LEFT, RGB(255, 255, 255), 0, 15, Str2, lpDDSBackBuffer);
          }

          int ScoreAmount = 0;
          int CollisionValue = Enemys.TestCollisions(&Player, &ScoreAmount);

          if (!MultiPlayer)
          {
            if (NewSecond && PlNewDmgDelay < 2)
              PlNewDmgDelay++;

            if (CollisionValue & PLAYER_IS_HIT)
            {
              if (PlNewDmgDelay > 1)
              {
                PlHealth--;

                if (PlHealth >= 1)
                {
                  i -= 1;
                  FXM.CreateExplosionAt(PlHlthPos.x + 65 * i, PlHlthPos.y - 20);
                  // DMM.PlaySndFX("Brrzz.wav");
                }

                if (PlHealth <= 0)
                {
                  FXM.CreateExplosionAt(
                    Player.GetXY(FALSE).x, Player.GetXY().y);
                  PlayerDieSequence = TRUE;
                }

                PlNewDmgDelay = 0;
              }
            }

            if (PlPoints > 99999999)
            {
              PlPoints = 99999999;
            }

            if (NewSecond && !(CollisionValue & PLAYER_IS_HIT))
            {
              PlPoints += 5;
            }

            if (CollisionValue & PLAYER_HAS_SCORED)
            {
              switch (ScoreAmount)
              {
                case 1:
                  PlPoints += 5;
                  break;
                case 2:
                  PlPoints += 10;
                  break;
                case 3:
                  PlPoints += 20;
                  break;
              }
            }
          }
          else // MultiPlayer == TRUE
          {
            if (NewSecond && MultiPlayerTime > 0)
              MultiPlayerTime--;

            if (MultiPlayerTime <= 0)
            {
              if (PlPoints != Pl2Pts)
              {
                GameOver = TRUE;
                PlayerDieSequence = TRUE;
              }
            }

            char Str[5];

            sprintf(
              Str, "%2d:%02d", MultiPlayerTime / 60, MultiPlayerTime % 60);

            ShowText(
              TA_CENTER, RGB(255, 255, 255), 380, 0, Str, lpDDSBackBuffer);

            int Pl2ScoreAmount;

            int Pl2CollisionValue =
              Enemys.TestCollisions(&Player2, &Pl2ScoreAmount);

            if (CollisionValue & PLAYER_IS_HIT)
              if (PlPoints > -20)
                PlPoints -= 10;

            if (CollisionValue & PLAYER_HAS_SCORED)
              PlPoints += ScoreAmount * 5;

            if (Pl2CollisionValue & PLAYER_IS_HIT)
              if (Pl2Pts > -20)
                Pl2Pts -= 10;

            if (Pl2CollisionValue & PLAYER_HAS_SCORED)
              Pl2Pts += Pl2ScoreAmount * 5;

            if (PlPoints > 99999999)
              PlPoints = 99999999;

            if (Pl2Pts > 99999999)
              Pl2Pts = 99999999;
          }
        }
        /* ----------> */ else if (PlayerDieSequence)
        {
          static short Times = 0;

          if (GameOver == FALSE)
          {
            Enemys.MoveAndShoot(FALSE);

            FXM.Draw();

            static short GameOverDelay = 0;

            if (++Times >= FrameRate / MSECS(500))
            {
              GameOverDelay++;
              Times = 0;
            }

            if (GameOverDelay == 2)
            {
              GameOver = TRUE;
              GameOverDelay = 0;
            }
          }
          else // GameOver == TRUE
          {
            RECT r = {0, 400, 500, 600};
            lpDDSBackBuffer->BltFast(
              155, 50, lpDDSLogo, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

            BOOL NewScoreEntry = FALSE;

            if (!MultiPlayer)
              NewScoreEntry = (PlPoints > LowestScore) ? TRUE : FALSE;

            static int RGBFadeIn = 0;

            if (RGBFadeIn <= 250 && ++Times >= FrameRate / MSECS(20))
            {
              Times = 0;
              RGBFadeIn += 5;
            }

            static BOOL DoFlush = TRUE, DoReset = FALSE;

            if (DoFlush && RGBFadeIn >= 250)
            {
              DIM.Flush();
              DoFlush = FALSE;
              Times = 0;
            }

            if (!MultiPlayer)
            {
              static char PlayerName[50];

              char str[25];
              sprintf(str, "Your Score is: %d", PlPoints);

              ShowText(
                TA_CENTER, RGB(255, 255, 255), 380, 250, str, lpDDSBackBuffer);

              if (NewScoreEntry)
              {
                static int WrittenChars = 0;

                if (RGBFadeIn >= 250)
                {
                  sprintf(str, "Enter your name:");
                  ShowText(
                    TA_CENTER,
                    RGB(255, 255, 255),
                    380,
                    280,
                    str,
                    lpDDSBackBuffer);
                }

                sprintf(str, "You made a new High Score!");
                ShowText(
                  TA_CENTER,
                  RGB(0, RGBFadeIn, 0),
                  380,
                  265,
                  str,
                  lpDDSBackBuffer);

                DWORD ScanCode = DIM.GetScanCode();

                unsigned short Typed = 0;

                if (ScanCode == DIK_BACK)
                {
                  if (WrittenChars)
                  {
                    PlayerName[WrittenChars - 1] = 0;
                    WrittenChars--;
                  }
                }
                else if (ScanCode == DIK_RETURN && PlayerName[0])
                {
                  int i;
                  for (i = 0; i < MAX_HISCORE_ENTRYS; i++)
                  {
                    if (Entrys[i].Score < PlPoints)
                      break;
                  }

                  if (i >= MAX_HISCORE_ENTRYS)
                  {
                    Error("Achtung: Eintrag in HighScore-Liste war \
												mĂ¶glich, obwohl Punktzahl nicht hoch genug war. \
												Dies sollt ĂĽberhaupt nicht mĂ¶glich sein, deshalb \
												muss ein interner Programmfehler vorliegen! \
												Programm wird beendet.");
                    DestroyWindow(hWnd);
                  }

                  for (int j = MAX_HISCORE_ENTRYS - 1; j > i; j--)
                  {
                    Entrys[j] = Entrys[j - 1];
                  }

                  strcpy(Entrys[i].Name, PlayerName);
                  Entrys[i].Score = PlPoints;

                  WrittenChars = 0;
                  strset(PlayerName, 0);
                  ViewScores = MarkPlacing = TRUE;
                  Placing = i;
                  LowestScore = Entrys[MAX_HISCORE_ENTRYS - 1].Score;

                  DoReset = TRUE;
                }

                Scan2Ascii(ScanCode, &Typed);

                if (
                  RGBFadeIn > 250 && Typed > 0 && strlen(PlayerName) < 49 &&
                  (isalpha(Typed) || isspace(Typed) || isalnum(Typed) ||
                   ispunct(Typed)) &&
                  ScanCode != DIK_RETURN)
                {
                  PlayerName[WrittenChars] = (char)Typed;
                  PlayerName[WrittenChars + 1] = 0;
                  WrittenChars++;
                }

                ShowText(
                  TA_CENTER,
                  RGB(255, 0, 0),
                  380,
                  300,
                  PlayerName,
                  lpDDSBackBuffer);
              }
              else // NewScoreEntry == FALSE
              {
                ShowText(
                  TA_CENTER,
                  RGB(RGBFadeIn, 0, 0),
                  380,
                  265,
                  "Sorry, you didn't make a High Score!",
                  lpDDSBackBuffer);
              }
            }
            else // MultiPlayer == TRUE
            {
              char Str[20];

              if (PlPoints > Pl2Pts)
                sprintf(
                  Str,
                  "Player 1 has won the match %d : %d !",
                  PlPoints,
                  Pl2Pts);
              else
                sprintf(
                  Str,
                  "Player 2 has won the match %d : %d !",
                  Pl2Pts,
                  PlPoints);

              ShowText(
                TA_CENTER,
                RGB(RGBFadeIn, 0, 0),
                380,
                265,
                Str,
                lpDDSBackBuffer);
            }

            if (DIM.GetScanCode() && !NewScoreEntry && RGBFadeIn >= 250)
              DoReset = TRUE;

            if (DoReset)
            {
              RGBFadeIn = 0;
              DoReset = FALSE;
              DoFlush = TRUE;
              InGame = FALSE;

              DIM.Flush();
              Menu.SetState(TRUE);
            }
          }
        }
      }
      else // Not in game
      {
        if (!ViewScores)
        {
          Menu.DrawMenu(lpDDSBackBuffer);

          switch (Menu.CheckMenu(DIM))
          {
            case 0:
              InGame = Running = TRUE;
              MultiPlayer = PlayerDieSequence = GameOver = FALSE;
              // ElapsedTime = 0;
              // LastTime    = timeGetTime();
              PlPoints = 0;
              PlHealth = PLAYER_MAX_HEALTH;

              Player.ClearShotList();
              Player.SetXY(350, 480);
              Enemys.ReInit(5);
              Menu.SetState(FALSE);
              DIM.Flush();
              break;
            case 1:
              MultiPlayerTime = MULTIPLAYER_START_TIME;

              InGame = MultiPlayer = Running = TRUE;
              PlayerDieSequence = GameOver = FALSE;
              // ElapsedTime          = 0;
              // LastTime             = timeGetTime();
              PlPoints = 0;
              Pl2Pts = 0;

              Player.ClearShotList();
              Player2.ClearShotList();
              Player.SetXY(450, 480);
              Player2.SetXY(250, 480);
              Enemys.ReInit(5);
              Menu.SetState(FALSE);
              DIM.Flush();
              break;
            case 2:
              ViewScores = TRUE;
              break;
            case 3:
              DestroyWindow(hWnd);
              break;
          }
        }
        else // ViewScores == TRUE
        {
          ShowText(
            TA_CENTER, RGB(255, 255, 255), 220, 180, "Name", lpDDSBackBuffer);
          ShowText(
            TA_CENTER, RGB(255, 255, 255), 580, 180, "Score", lpDDSBackBuffer);

          for (int i = 0; i < MAX_HISCORE_ENTRYS; i++)
          {
            char strScore[9] = "";

            itoa(Entrys[i].Score, strScore, 10);

            COLORREF Color = RGB(255, 0, 0);

            if (MarkPlacing && i == Placing)
            {
              Color = RGB(0, 255, 0);
            }

            ShowText(
              TA_LEFT,
              Color,
              200,
              200 + i * 20,
              Entrys[i].Name,
              lpDDSBackBuffer);
            ShowText(
              TA_RIGHT, Color, 600, 200 + i * 20, strScore, lpDDSBackBuffer);
          }

          static int RGBFade = 0;

          static BOOL bDoInc = TRUE;

          if (RGBFade >= 248)
          {
            bDoInc = FALSE;
          }
          else if (RGBFade <= 0)
          {
            bDoInc = TRUE;
          }

          static short Times = FrameRate / MSECS(500);

          if (++Times > FrameRate / MSECS(20))
          {
            if (bDoInc)
            {
              RGBFade += 8;
            }
            else
            {
              RGBFade -= 8;
            }

            Times = 0;
          }

          ShowText(
            TA_CENTER,
            RGB(RGBFade, RGBFade, RGBFade),
            410,
            220 + MAX_HISCORE_ENTRYS * 20,
            "Press any key to return to main menu",
            lpDDSBackBuffer);

          if (DIM.GetScanCode())
          {
            ViewScores = MarkPlacing = FALSE;
          }
        }
      }

      DDM.Flip();
    }
  }

  fScoreList = fopen("Scores.dat", "wb");

  if (nullptr != fScoreList)
  {
    fwrite(Entrys, sizeof(HiScoreEntry), MAX_HISCORE_ENTRYS, fScoreList);
    fclose(fScoreList);
  }

  return msg.wParam;
}
