#include "GameManager.h"

static LPDIRECTDRAWSURFACE7 lpDDSEnemy1;
static LPDIRECTDRAWSURFACE7 lpDDSEnemy2;
static LPDIRECTDRAWSURFACE7 lpDDSEnemy3;
static LPDIRECTDRAWSURFACE7 lpDDSEmShot;

void SpriteManager::MoveAndShoot(BOOL Move)
{
  if (NrSprites < MAX_ENEMYS && Move == TRUE)
  {
    if (NrSprites == 0)
      AddEnemys(1);

    if (++Times > FrameRate / 60)
    {
      NewSprTime++;
      Times = 0;
    }

    if (NewSprTime > NEWSPR_DELAY)
    {
      AddEnemys(1);
      NewSprTime = 0;
    }
  }

  SpriteList.FirstKnoten();

  ASprite* Spr = nullptr;
  Sprite* ShotsToDel[MAX_SHOTS];
  unsigned char Index = 0;

  while (nullptr != SpriteList.GetCurrent())
  {
    Spr = static_cast<ASprite*>(SpriteList.GetCurrent()->GetEntry());

    if (Spr->GetType() == 4)
    {
      if (
        Spr->GetXY().y - Spr->GetSize().y > 0 &&
        Spr->GetXY().y + Spr->GetSize().y < SCR_HEIGHT - 10)
      {
        Spr->Move(0, SHOTSPEED);
        Spr->Draw(lpDDS);
      }
      else
      {
        ShotsToDel[Index] = Spr;
        Index++;
      }
    }
    else
    {
      if (Move)
        Spr->Bounce();

      Spr->Draw(lpDDS);
      Spr->IncShotDelay();

      if (rand() % 150 == 0 && Move == TRUE)
      {
        if (Spr->DoFire(&DMM))
        {
          FireShot(Spr->GetXY().x, Spr->GetXY().y + Spr->GetSize().y);
        }
      }
    }

    if (SpriteList.NextKnoten() == FALSE)
    {
      break;
    }
  }

  for (int i = 0; i < Index; i++)
  {
    SpriteList.FirstKnoten();

    while ((static_cast<Sprite*>(SpriteList.GetCurrent()->GetEntry())) !=
           ShotsToDel[i])
    {
      if (!SpriteList.NextKnoten())
        goto end;
    }

    SpriteList.DeleteKnoten();
    NrShots--;

  end:;
  }
}

void SpriteManager::Init(int NrEnemys, LPDIRECTDRAWSURFACE7 lpDDS)
{
  if (nullptr == lpDDSEnemy1)
    lpDDSEnemy1 = DDM.CreateSurfaceFromBitmap("Sprites\\Enemy1.bmp", 240, 60);

  if (nullptr == lpDDSEnemy2)
    lpDDSEnemy2 = DDM.CreateSurfaceFromBitmap("Sprites\\Enemy2.bmp", 45, 40);

  if (nullptr == lpDDSEnemy3)
    lpDDSEnemy3 = DDM.CreateSurfaceFromBitmap("Sprites\\Enemy3.bmp", 280, 100);

  if (nullptr == lpDDSEmShot)
    lpDDSEmShot = DDM.CreateSurfaceFromBitmap("Sprites\\EmShoot.bmp", 60, 20);

  if (nullptr == this->lpDDS)
    this->lpDDS = lpDDS;

  AddEnemys(NrEnemys);
}

int SpriteManager::TestCollisions(ArmedSprite* ToTest, int* pScoreAmount)
{
  int ret = 0;

  SpriteList.FirstKnoten();

  ASprite* Spr;

  for (int i = 0; i < NrSprites + NrShots; i++)
  {
    Spr = static_cast<ASprite*>(SpriteList.GetCurrent()->GetEntry());

    char NrQuad1 = 1, NrQuad2 = 1;

    if (ToTest->GetXY(FALSE).x > SCR_WIDTH / 2)
      NrQuad1 = 2;

    if (ToTest->GetXY().y < SCR_HEIGHT / 2)
      NrQuad1 += 2;

    if (Spr->GetXY(FALSE).x > SCR_WIDTH / 2)
      NrQuad2 = 2;

    if (Spr->GetXY().y < SCR_HEIGHT / 2)
      NrQuad2 += 2;

    if (NrQuad1 == NrQuad2)
    {
      if (ToTest->TestCollision(Spr) == TRUE)
      {
        if (Spr->GetType() == 4)
        {
          NrShots--;
        }
        else
        {
          FXM.CreateExplosionAt(Spr->GetXY(FALSE).x, Spr->GetXY().y);
          NrSprites--;
        }

        SpriteList.DeleteKnoten();

        ret += PLAYER_IS_HIT;
      }
    }

    if (Spr->GetType() != 4)
    {
      if (ToTest->TestShotCollision(Spr) == TRUE)
      {
        char Type = Spr->GetType();

        FXM.CreateExplosionAt(Spr->GetXY(FALSE).x, Spr->GetXY().y);

        SpriteList.DeleteKnoten();

        NrSprites--;

        ret += PLAYER_HAS_SCORED;

        if (nullptr != pScoreAmount)
        {
          *pScoreAmount = Type;
        }
      }
    }

    if (SpriteList.NextKnoten() == FALSE)
    {
      break;
    }
  }

  return ret;
}

void SpriteManager::ReInit(int NrEnemys)
{
  if (NrShots > 0)
  {
    while (nullptr != SpriteList.GetCurrent())
    {
      if (
        (static_cast<Sprite*>(SpriteList.GetCurrent()->GetEntry()))
          ->GetType() == 4)
      {
        SpriteList.DeleteKnoten();
        NrShots--;
      }

      if (!SpriteList.NextKnoten())
        break;
    }
  }

  NewSprTime = 0;

  while (NrSprites > NrEnemys)
  {
    SpriteList.LastKnoten();
    SpriteList.DeleteKnoten();
    NrSprites--;
  }

  if (NrSprites < NrEnemys)
  {
    AddEnemys(NrEnemys - NrSprites);
  }
}

void SpriteManager::AddEnemys(int Nr)
{
  for (int i = 0; i < Nr; i++)
  {
    if (NrSprites < MAX_ENEMYS)
    {
      char Rnd = static_cast<char>(rand() % 3 + 1);

      auto* Spr = new ASprite;

      switch (Rnd)
      {
        case 1:
          Spr->Create(lpDDSEnemy1, 60, 60, MSECS(40), TRUE, 50, 0);
          Spr->SetData(1, "Enemy1.wav", 10);
          Spr->SetNewBBox(8, 10, 51, 49);
          break;
        case 2:
          Spr->Create(lpDDSEnemy2, 45, 40, 0, TRUE, 75, 75);
          Spr->SetData(2, "Enemy2.wav", 5);
          Spr->SetNewBBox(5, 4, 38, 35);
          break;
        case 3:
          Spr->Create(lpDDSEnemy3, 70, 100, MSECS(50), TRUE, 50, 150);
          Spr->SetData(3, "Enemy3.wav", 2);
          Spr->SetNewBBox(9, 29, 62, 99);
          break;
        default:
          Spr->Create(lpDDSEnemy1, 60, 60, MSECS(35), TRUE, 50, 0);
          Spr->SetData(1, "Enemy1.wav", 10);
          Spr->SetNewBBox(8, 10, 52, 49);
          break;
      }

      Spr->SetXY(
        (rand() % SCR_WIDTH + 1), ((rand() % SCR_HEIGHT + 1) - SCR_HEIGHT / 2));

      SpriteList.Add((void*)Spr);
      NrSprites++;
    }
  }
}

void SpriteManager::FireShot(int x, int y)
{
  if (NrShots < MAX_SHOTS)
  {
    auto* Shot = new Sprite;

    Shot->Create(lpDDSEmShot, 12, 20, MSECS(16), TRUE);

    Shot->SetXY(x, y);
    Shot->SetType(4);

    SpriteList.Add((void*)Shot);

    NrShots++;
  }
}
