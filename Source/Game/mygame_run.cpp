#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"
#include "sstream"

using namespace game_framework;

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////

CGameStateRun::CGameStateRun(CGame* g) : CGameState(g)
{

}

CGameStateRun::~CGameStateRun()
{
}

void CGameStateRun::OnBeginState()
{
}

void CGameStateRun::OnMove()							// 移動遊戲元素
{
	if (this->page_phase >= 6){
		fireman.IsMoving(this->map, this->mapPole, this->page_phase - 5);
		watergirl.IsMoving(this->map, this->mapPole, this->page_phase - 5);
		if (!fireman.IsUpButtonClick) 
			this->fireman.isDropDown(this->map, this->mapPole, this->mapBox, page_phase - 5);
		if (!watergirl.IsWButtonClick) 
			this->watergirl.isDropDown(this->map, this->mapPole, this->mapBox, page_phase - 5);
		this->runOverlap(this->page_phase - 5);
	}
}

void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
{	
	/*
	background[3]: home, menu, map
	window[4]: setting, paused, die, pass
	*/
	scene.loadScene();

	/*
	home: (0)play  (1)setting
	menu: (2)stage1, 2, 3, 4 (3)back
	map: (4)paused
	window-
	settig: (5)muic (6)effect (7)end
	paused: (8)end (9)resume (10)retry
	die: (11)menu (12)retry (13)skip
	pass: (14)continue
	*/
	button.loadButton();

	fireman.character.LoadBitmapByString({ "Resources/characters/fireman_front_1.bmp", "Resources/characters/fireman_front_2.bmp", "Resources/characters/fireman_front_3.bmp",
										"Resources/characters/fireman_front_4.bmp", "Resources/characters/fireman_front_5.bmp", "Resources/characters/fireman_left_1.bmp",
										"Resources/characters/fireman_left_2.bmp", "Resources/characters/fireman_left_3.bmp", "Resources/characters/fireman_left_4.bmp",
										"Resources/characters/fireman_left_5.bmp", "Resources/characters/fireman_right_1.bmp", "Resources/characters/fireman_right_2.bmp",
										"Resources/characters/fireman_right_3.bmp", "Resources/characters/fireman_right_4.bmp", "Resources/characters/fireman_right_5.bmp" }, RGB(0, 255, 0));
	fireman.character.SetTopLeft(38, 877);

	watergirl.character.LoadBitmapByString({ "Resources/characters/watergirl_front_1.bmp", "Resources/characters/watergirl_front_2.bmp", "Resources/characters/watergirl_front_3.bmp",
											"Resources/characters/watergirl_front_4.bmp", "Resources/characters/watergirl_front_5.bmp", "Resources/characters/watergirl_left_1.bmp",
											"Resources/characters/watergirl_left_2.bmp", "Resources/characters/watergirl_left_3.bmp", "Resources/characters/watergirl_left_4.bmp",
											"Resources/characters/watergirl_left_5.bmp", "Resources/characters/watergirl_right_1.bmp", "Resources/characters/watergirl_right_2.bmp",
											"Resources/characters/watergirl_right_3.bmp", "Resources/characters/watergirl_right_4.bmp", "Resources/characters/watergirl_right_5.bmp" }, RGB(0, 255, 0));
	watergirl.character.SetTopLeft(38, 737);

	map.generateMap();
	mapButton.generateObject();
	mapController.generateObject();
	mapPole.generateObject();
	mapBox.generateObject();
	mapDoor.generateObject();
	mapDiamond.generateObject();
	mapPool.generateObject();
	mapFan.generateObject();
	for (int i = 0; i < 2; i++){
		wind[i].LoadBitmapByString({"Resources/wind/wind_1.bmp", "Resources/wind/wind_2.bmp", "Resources/wind/wind_3.bmp", "Resources/wind/wind_4.bmp"
								, "Resources/wind/wind_5.bmp", "Resources/wind/wind_6.bmp", "Resources/wind/wind_7.bmp", "Resources/wind/wind_8.bmp"
								, "Resources/wind/wind_9.bmp", "Resources/wind/wind_10.bmp","Resources/wind/wind_11.bmp", "Resources/wind/wind_12.bmp"
								, "Resources/wind/wind_13.bmp", "Resources/wind/wind_14.bmp", "Resources/wind/wind_15.bmp", "Resources/wind/wind_16.bmp"
								, "Resources/wind/wind_17.bmp", "Resources/wind/wind_18.bmp", "Resources/wind/wind_19.bmp", "Resources/wind/wind_20.bmp"
								, "Resources/wind/wind_21.bmp", "Resources/wind/wind_22.bmp", "Resources/wind/wind_23.bmp", "Resources/wind/wind_24.bmp"
								, "Resources/wind/wind_25.bmp", "Resources/wind/wind_26.bmp", "Resources/wind/wind_27.bmp", "Resources/wind/wind_28.bmp"
								, "Resources/wind/wind_29.bmp", "Resources/wind/wind_30.bmp" }, RGB(30, 30, 30));
		wind[i].SetAnimation(200, false);
	}
	
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	fireman.IsButtonDown(nChar);
	watergirl.IsButtonDown(nChar);

	// 按下I，啟動無敵
	const int VK_I = 0x49;
	if (nChar == VK_I) 
		this->invincible += 1;
	// 按下O，關閉人物動畫
	const int VK_O = 0x4F;
	if (nChar == VK_O)
		this->animation += 1;
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	fireman.IsButtonUp(nChar);
	watergirl.IsButtonUp(nChar);
}

void CGameStateRun::OnLButtonDown(UINT nFlags, CPoint point)  // 處理滑鼠的動作
{
	if (nFlags == VK_LBUTTON) {
		IsMouseOverlap(point.x, point.y);
	}
}

void CGameStateRun::OnLButtonUp(UINT nFlags, CPoint point)	// 處理滑鼠的動作
{
	const auto audio = CAudio::Instance();
	if (buttonClick) {
		buttonClick = false;
		button.buttonUp(click_button);
		//button click effect
		if (effectPlay) {
			audio->Play(5, false);
		}

		switch (click_button) {
			case 0:
				//plauButton at Home
				page_phase = 1;
				if (musicPlay) {
					audio->Stop(0);
					audio->Stop(1);
					audio->Play(2, true);
				}
				break;
			case 1:
				//settingButton at Home
				page_phase = 2;
				if (musicPlay) {
					audio->Stop(0);
					audio->Stop(1);
					audio->Play(2, true);
				}
				break;
			case 7 :
				//backButton at Menu
				page_phase = 0;
				if (musicPlay) {
					audio->Stop(1);
					audio->Stop(2);
					audio->Play(0, true);
				}
				break;
			case 8:
				//pausedButton at Map
				page_phase = 3;
				break;
			case 9:
				//musicButton at setting
				if (musicPlay) {
					audio->Pause();
					musicPlay = false;
				}
				else {
					musicPlay = true;
				}
				break;
			case 10:
				//effectButton at setting
				if (effectPlay) {
					effectPlay = false;
				}
				else {
					effectPlay = true;
				}
				break;
			case 11:
				//backButton at setting
				page_phase = 0;
				if (musicPlay) {
					audio->Stop(1);
					audio->Stop(2);
					audio->Play(0, true);
				}
				break;
			case 12:
				//endButton at paused
				page_phase = 1;
				if (musicPlay) {
					audio->Stop(0);
					audio->Stop(2);
					audio->Play(1, true);
				}
				break;
			case 13:
				//resumeButton at paused
				this->page_phase = last_stage;
				break;
			case 14:
				//retryButton at paused
				buttonClick = true;
				this->resetLevel(last_stage - 5);
				this->page_phase = last_stage;
				break;
			case 15:
				//munuButton at die
				page_phase = 1;
				if (musicPlay) {
					audio->Stop(0);
					audio->Stop(2);
					audio->Play(1, true);
				}
				this->resetLevel(last_stage - 5);
				break;
			case 16:
				//retruButton at die
				this->resetLevel(last_stage - 5);
				this->page_phase = last_stage;
				break;
			case 17:
				//skipButton at die
				page_phase = 1;
				break;
			case 18:
				//continueButton at pass
				page_phase = 1;
				if (musicPlay) {
					audio->Stop(0);
					audio->Stop(2);
					audio->Play(1, true);
				}
				break;
		}
	}
	

}

void CGameStateRun::OnMouseMove(UINT nFlags, CPoint point)	// 處理滑鼠的動作
{

}

void CGameStateRun::OnRButtonDown(UINT nFlags, CPoint point)  // 處理滑鼠的動作
{
}

void CGameStateRun::OnRButtonUp(UINT nFlags, CPoint point)	// 處理滑鼠的動作
{
}

void CGameStateRun::OnShow()
{
	
	

	scene.showScene(page_phase);

	

	if (page_phase >= 6) {	//Map
		last_stage = page_phase;	//紀錄所在關卡 用於重製&繼續
		map.showMap(page_phase - 5);
		mapButton.showObject(page_phase - 5);
		mapController.showObject(page_phase - 5);
		mapPole.showObject(page_phase - 5);
		mapBox.showObject(page_phase - 5);
		mapDoor.showObject(page_phase - 5);
		mapDiamond.showObject(page_phase - 5);
		mapPool.showObject(page_phase - 5);
		mapFan.showObject(page_phase - 5);

		if (this->animation % 2 == 0){
			fireman.SetFrame();
			watergirl.SetFrame();
		}
		fireman.character.ShowBitmap();		//(38, 877)
		watergirl.character.ShowBitmap();	//(38, 737)
	}
	if (page_phase == 9) {
		wind[0].SetTopLeft(55, 240);
		wind[0].ShowBitmap();
		wind[1].SetTopLeft(1245, 240);
		wind[1].ShowBitmap();
	}
	button.showButton(page_phase);

	if (this->invincible % 2 == 1) {
		CDC* pdc = CDDraw::GetBackCDC();
		CTextDraw::Print(pdc, 630, 10, "開啟無敵模式");
		CDDraw::ReleaseBackCDC();
	}

	if (this->animation % 2 == 1) {
		CDC* pdc = CDDraw::GetBackCDC();
		CTextDraw::Print(pdc, 630, 30, "關閉人物動畫");
		CDDraw::ReleaseBackCDC();
	}

	ShowWindowCoordinate();
}


void CGameStateRun::ShowWindowCoordinate() {
	// 座標轉成字串
	string text;
	stringstream ss;
	// 宣告顯示文字工具
	CDC* pDC = CDDraw::GetBackCDC();
	// Window 底下的API
	POINT pt;							// 座標
	HWND hWnd = GetForegroundWindow();	// 宣告並取得當前視窗的句柄
	BOOL bReturn = GetCursorPos(&pt);	// 取得當前鼠標在Screen上的座標(若無return false)
	ScreenToClient(hWnd, &pt);			// 將取得Screen的座標轉成當前視窗句柄的座標
	if (bReturn != 0) {
		ss << pt.x << ", " << pt.y;
		text = ss.str();
		CTextDraw::Print(pDC, 5, 5, text);
	}
	CDDraw::ReleaseBackCDC();
}



void CGameStateRun::IsMouseOverlap(int mouse_x, int mouse_y) {
	const auto audio = CAudio::Instance();
	buttonClick = false;
	
	//playButton at Home
	if (button.ifOverlap(0, CPoint(mouse_x, mouse_y)) && page_phase == 0) {
		click_button = 0;
		buttonClick = true;
	}
	//settingButton at Home
	if (button.ifOverlap(1, CPoint(mouse_x, mouse_y)) && page_phase == 0) {
		click_button = 1;
		buttonClick = true;
	}

	//stageButton at Menu
	for (int i = 2; i < 7; i++) {
		if (button.ifOverlap(i, CPoint(mouse_x, mouse_y)) && page_phase == 1) {
			this->page_phase = i + 4;
			if (musicPlay) {
				audio->Stop(0);
				audio->Stop(1);
				audio->Play(2, true);
			}
			click_button = i;
			buttonClick = true;
			this->resetLevel(this->page_phase - 5);
			break; // let stageButton decide once
		}
	}
	//backButton at Menu
	if (button.ifOverlap(7, CPoint(mouse_x, mouse_y)) && page_phase == 1) {
		click_button = 7;
		buttonClick = true;
	}
	//pausedButton at Map
	if (button.ifOverlap(8, CPoint(mouse_x, mouse_y)) && page_phase >= 6) {
		click_button = 8;
		buttonClick = true;
	}

	if (button.ifOverlap(9, CPoint(mouse_x, mouse_y)) && page_phase == 2) {
		//music on/off
		click_button = 9;
		buttonClick = true;
		
	}
	if (button.ifOverlap(10, CPoint(mouse_x, mouse_y)) && page_phase == 2) {
		//effect on/of
		click_button = 10;
		buttonClick = true;
	}
	//backButton at setting
	if (button.ifOverlap(11, CPoint(mouse_x, mouse_y)) && page_phase == 2) {
		click_button = 11;
		buttonClick = true;
	}
	//endButton at paused
	if (button.ifOverlap(12, CPoint(mouse_x, mouse_y)) && page_phase == 3) {
		click_button = 12;
		buttonClick = true;
	}
	//resumeButton at paused
	if (button.ifOverlap(13, CPoint(mouse_x, mouse_y)) && page_phase == 3) {
		click_button = 13;
		buttonClick = true;
	}
	if (button.ifOverlap(14, CPoint(mouse_x, mouse_y)) && page_phase == 3) {
		click_button = 14;
		buttonClick = true;
	}
	//menuButton at die
	if (button.ifOverlap(15, CPoint(mouse_x, mouse_y)) && page_phase == 4) {
		click_button = 15;
		buttonClick = true;
	}
	//retryButton at die
	if (button.ifOverlap(16, CPoint(mouse_x, mouse_y)) && page_phase == 4) {
		click_button = 16;
		buttonClick = true;
	}
	//skipButton at die
	if (button.ifOverlap(17, CPoint(mouse_x, mouse_y)) && page_phase == 4) {
		click_button = 17;
		buttonClick = true;
	}
	//continueButton at pass
	if (button.ifOverlap(18, CPoint(mouse_x, mouse_y)) && page_phase == 5) {
		click_button = 18;
		buttonClick = true;
	}

	if (buttonClick) {
		button.buttonDown(click_button);
	}
}

// reset level's everything when clicks the button of retry
void CGameStateRun::resetLevel(int page) {
	this->fireman.resetMap(page);
	this->watergirl.resetMap(page);
	this->mapBox.resetMap(page);
	this->mapButton.resetMap(page);
	this->mapController.resetMap(page);
	this->mapDiamond.resetMap(page);
	this->mapDoor.resetMap(page);
	this->mapPole.resetMap(page);
}

void CGameStateRun::isControllerOverlap(int page, CMovingBitmap& character) {
	if (page == 1) {
		// overlap from right side (opening controller)
		if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[0])
			&& CMovingBitmap::IsOverlap(character, this->mapController.comboBox[0])
			&& character.GetLeft() < this->mapController.mapController[0].GetLeft()){
			this->mapController.mapController[0].SetFrameIndexOfBitmap(1);
			this->mapController.controllerState[0] = true;
			this->controllerMode = 1;
		}
		// overlap from left side (closing controller)
		else if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[0])
			&& CMovingBitmap::IsOverlap(character, this->mapController.comboBox[0])
			&& character.GetLeft() + 20 > this->mapController.mapController[0].GetLeft() + 38){
			this->mapController.mapController[0].SetFrameIndexOfBitmap(0);
			this->mapController.controllerState[0] = true;
			this->controllerMode = 2;
		}

		if (this->mapController.controllerState[0]) {
			this->movingPole(1, 0, character);
		}
	}

	if (page == 3) {
		for (int i = 0; i < 4; i++) {
			if (i == 0 || i == 3) {
				// overlap from right side (opening controller)
				if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[i])
					&& character.GetLeft() < this->mapController.mapController[i].GetLeft()) {
					this->mapController.mapController[i].SetFrameIndexOfBitmap(1);
					this->mapController.controllerState[i] = true;
					this->controllerMode = 1;
				}
				// overlap from left side (closing controller)
				else if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[i])
					&& character.GetLeft() + 20 > this->mapController.mapController[i].GetLeft() + 38) {
					this->mapController.mapController[i].SetFrameIndexOfBitmap(0);
					this->mapController.controllerState[i] = true;
					this->controllerMode = 2;
				}
			}
			if (i == 1 || i == 2) {
				if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[i])) {
					this->mapController.mapController[i].SetFrameIndexOfBitmap(1);
					this->mapController.controllerState[i] = true;
					this->controllerMode = 1;
				}
			}
			if (this->mapController.controllerState[i])
				this->movingPole(3, i, character);
		}
	}

	if (page == 5) {
		for (int i = 1; i < 6; i++) {
			if (i != 5) {
				// overlap from right side (opening controller)
				if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[i])
					&& CMovingBitmap::IsOverlap(character, this->mapController.comboBox[i])
					&& character.GetLeft() < this->mapController.mapController[i].GetLeft()) {
					this->mapController.mapController[i].SetFrameIndexOfBitmap(1);
					this->mapController.controllerState[i] = true;
					this->ctrlMode[i] = 1;
				}
				// overlap from left side (closing controller)
				else if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[i])
					&& CMovingBitmap::IsOverlap(character, this->mapController.comboBox[i])
					&& character.GetLeft() + 20 > this->mapController.mapController[i].GetLeft() + 38) {
					this->mapController.mapController[i].SetFrameIndexOfBitmap(0);
					this->mapController.controllerState[i] = true;
					this->ctrlMode[i] = 2;
				}
			}
			if (i == 5) {
				// overlap from right side (opening controller)
				if (CMovingBitmap::IsOverlap(character, this->mapController.mapController[i])
					&& character.GetLeft() < this->mapController.mapController[i].GetLeft()) {
					this->mapController.mapController[i].SetFrameIndexOfBitmap(1);
					this->mapController.controllerState[i] = true;
					this->ctrlMode[i] = 1;
				}
			}
			if (this->mapController.controllerState[i])
				this->movingPole(5, i, character);
		}
	}
	
}

void CGameStateRun::movingPole(int page, int index, CMovingBitmap &character) {
	int current_Height;
	if (page == 1) {
		current_Height = this->mapPole.mapPole[index].GetTop();

		if (current_Height < 655 && this->controllerMode == 1) {
			this->mapPole.mapPole[index].SetTopLeft(this->mapPole.mapPole[index].GetLeft(), current_Height + 3);
		}
		else if (current_Height > 525 && this->controllerMode == 2) {
			this->mapPole.mapPole[index].SetTopLeft(this->mapPole.mapPole[index].GetLeft(), current_Height - 3);
		}
		else {
			this->mapController.controllerState[0] = false;
			this->controllerMode = 0;
		}
	}
	if (page == 3) {
		if (index == 0) { // yellow controller
			if (this->mapPole.mapPole[10].GetLeft() > 560 && this->controllerMode == 1)
				this->mapPole.mapPole[10].SetTopLeft(this->mapPole.mapPole[10].GetLeft() - 3, this->mapPole.mapPole[10].GetTop());
			else if (this->mapPole.mapPole[10].GetLeft() < 700 && this->controllerMode == 2)
				this->mapPole.mapPole[10].SetTopLeft(this->mapPole.mapPole[10].GetLeft() + 3, this->mapPole.mapPole[10].GetTop());
			else {
				this->mapController.controllerState[index] = false;
				this->controllerMode = 0;
			}
		}
		if (index == 1) { // white controller
			if (this->mapPole.mapPole[3].GetLeft() > 1015 && this->controllerMode == 1)
				this->mapPole.mapPole[3].SetTopLeft(this->mapPole.mapPole[3].GetLeft() - 3, this->mapPole.mapPole[3].GetTop());
			else if (this->mapPole.mapPole[3].GetLeft() < 1155 && this->controllerMode == 2)
				this->mapPole.mapPole[3].SetTopLeft(this->mapPole.mapPole[3].GetLeft() + 3, this->mapPole.mapPole[3].GetTop());
			else {
				this->mapController.controllerState[index] = false;
				this->controllerMode = 0;
			}
		}
		if (index == 2) { // green controller
			if (this->mapPole.mapPole[6].GetTop() < 700 && this->controllerMode == 1)
				this->mapPole.mapPole[6].SetTopLeft(this->mapPole.mapPole[6].GetLeft(), this->mapPole.mapPole[6].GetTop() + 3);
			else if (this->mapPole.mapPole[6].GetTop() > 595 && this->controllerMode == 2)
				this->mapPole.mapPole[6].SetTopLeft(this->mapPole.mapPole[6].GetLeft(), this->mapPole.mapPole[6].GetTop() - 3);
			else {
				this->mapController.controllerState[index] = false;
				this->controllerMode = 0;
			}
		}
		if (index == 3) { // purple controller
			// 因為紫色控制桿與其他方向相反，controllerMode也跟著相反
			if (this->controllerMode == 2) { // closing
				if (this->mapPole.mapPole[7].GetLeft() < 665) {  // No. 7
					this->mapPole.mapPole[7].SetTopLeft(this->mapPole.mapPole[7].GetLeft() + 3, this->mapPole.mapPole[7].GetTop());
					// 角色在升降梯移動時，會被推走
					if (CMovingBitmap::IsOverlap(character, this->mapPole.mapPole[7]))
						character.SetTopLeft(this->mapPole.mapPole[7].GetLeft() + this->mapPole.mapPole[7].GetWidth(), character.GetTop());
				}
				if (this->mapPole.mapPole[11].GetLeft() < 805) { // No. 11
					this->mapPole.mapPole[11].SetTopLeft(this->mapPole.mapPole[11].GetLeft() + 3, this->mapPole.mapPole[11].GetTop());
					// 角色在升降梯移動時，會被推走
					if (CMovingBitmap::IsOverlap(character, this->mapPole.mapPole[11]))
						character.SetTopLeft(this->mapPole.mapPole[11].GetLeft() + this->mapPole.mapPole[11].GetWidth(), character.GetTop());
				}
			}
			else if (this->controllerMode == 1) { // opening
				if (this->mapPole.mapPole[7].GetLeft() > 530) {  // No. 7
					this->mapPole.mapPole[7].SetTopLeft(this->mapPole.mapPole[7].GetLeft() - 3, this->mapPole.mapPole[7].GetTop());
					// 角色在升降梯移動時，會被推走
					if (CMovingBitmap::IsOverlap(character, this->mapPole.mapPole[7]))
						character.SetTopLeft(this->mapPole.mapPole[7].GetLeft() - character.GetWidth(), character.GetTop());
				}
				if (this->mapPole.mapPole[11].GetLeft() > 665) { // No. 11
					this->mapPole.mapPole[11].SetTopLeft(this->mapPole.mapPole[11].GetLeft() - 3, this->mapPole.mapPole[11].GetTop());
					// 角色在升降梯移動時，會被推走
					if (CMovingBitmap::IsOverlap(character, this->mapPole.mapPole[11]))
						character.SetTopLeft(this->mapPole.mapPole[11].GetLeft() - character.GetWidth(), character.GetTop());
				}
			}
			else {
				this->mapController.controllerState[index] = false;
				this->controllerMode = 0;
			}
		}
	}

	if (page == 5) {
		if (index == 1) { // white controller
			if (this->ctrlMode[index] == 1) { // opening
				if (this->mapPole.mapPole[9].GetTop() > 280) // No. 9
					this->mapPole.mapPole[9].SetTopLeft(this->mapPole.mapPole[9].GetLeft(), this->mapPole.mapPole[9].GetTop() - 3);
				if (this->mapPole.mapPole[12].GetTop() > 420) // No. 12
					this->mapPole.mapPole[12].SetTopLeft(this->mapPole.mapPole[12].GetLeft(), this->mapPole.mapPole[12].GetTop() - 3);
				else
					this->mapController.controllerState[index] = false;
				if (this->mapPole.mapPole[14].GetLeft() > 805) // No. 14
					this->mapPole.mapPole[14].SetTopLeft(this->mapPole.mapPole[14].GetLeft() - 3, this->mapPole.mapPole[14].GetTop());
				else
					this->mapController.controllerState[index] = false;
				if (this->mapPole.mapPole[15].GetLeft() > 665) // No. 15
					this->mapPole.mapPole[15].SetTopLeft(this->mapPole.mapPole[15].GetLeft() - 3, this->mapPole.mapPole[15].GetTop());
				else 
					this->mapController.controllerState[index] = false;
			}
			else if (this->ctrlMode[index] == 2) { // closing
				if (this->mapPole.mapPole[9].GetTop() < 420) // No. 9
					this->mapPole.mapPole[9].SetTopLeft(this->mapPole.mapPole[9].GetLeft(), this->mapPole.mapPole[9].GetTop() + 3);
				if (this->mapPole.mapPole[12].GetTop() < 560) // No. 12
					this->mapPole.mapPole[12].SetTopLeft(this->mapPole.mapPole[12].GetLeft(), this->mapPole.mapPole[12].GetTop() + 3);
				if (this->mapPole.mapPole[14].GetLeft() < 945) // No. 14
					this->mapPole.mapPole[14].SetTopLeft(this->mapPole.mapPole[14].GetLeft() + 3, this->mapPole.mapPole[14].GetTop());
				if (this->mapPole.mapPole[15].GetLeft() < 805) // No. 15
					this->mapPole.mapPole[15].SetTopLeft(this->mapPole.mapPole[15].GetLeft() + 3, this->mapPole.mapPole[15].GetTop());
			}
			else {
				this->mapController.controllerState[index] = false;
				this->ctrlMode[index] = 0;
			}
		}
		if (index == 2) { // green controller
			if (this->ctrlMode[index] == 1) { // opening
				if (this->mapPole.mapPole[5].GetTop() < 420) // No. 5
					this->mapPole.mapPole[5].SetTopLeft(this->mapPole.mapPole[5].GetLeft(), this->mapPole.mapPole[5].GetTop() + 3);
				if (this->mapPole.mapPole[6].GetLeft() > 420) // No. 6
					this->mapPole.mapPole[6].SetTopLeft(this->mapPole.mapPole[6].GetLeft() - 3, this->mapPole.mapPole[6].GetTop());
				if (this->mapPole.mapPole[16].GetTop() > 560) // No. 16
					this->mapPole.mapPole[16].SetTopLeft(this->mapPole.mapPole[16].GetLeft(), this->mapPole.mapPole[16].GetTop() - 3);
			}
			else if (this->ctrlMode[index] == 2) { // closing
				if (this->mapPole.mapPole[5].GetTop() > 280) // No. 5
					this->mapPole.mapPole[5].SetTopLeft(this->mapPole.mapPole[5].GetLeft(), this->mapPole.mapPole[5].GetTop() - 3);
				if (this->mapPole.mapPole[6].GetLeft() < 560) // No. 6
					this->mapPole.mapPole[6].SetTopLeft(this->mapPole.mapPole[6].GetLeft() + 3, this->mapPole.mapPole[6].GetTop());
				if (this->mapPole.mapPole[16].GetTop() < 700) // No. 16
					this->mapPole.mapPole[16].SetTopLeft(this->mapPole.mapPole[16].GetLeft(), this->mapPole.mapPole[16].GetTop() + 3);
			}
			else {
				this->mapController.controllerState[index] = false;
				this->ctrlMode[index] = 0;
			}
		}
		if (index == 3) { // purple controller
			if (this->ctrlMode[index] == 1) { // opening
				if (this->mapPole.mapPole[7].GetLeft() > 770){
					this->mapPole.mapPole[7].SetTopLeft(this->mapPole.mapPole[7].GetLeft() - 3, this->mapPole.mapPole[7].GetTop());
					// 角色在升降梯移動時，會被推走
					if (CMovingBitmap::IsOverlap(character, this->mapPole.mapPole[7]))
						character.SetTopLeft(this->mapPole.mapPole[7].GetLeft() - character.GetWidth(), character.GetTop());
				}
				if (this->mapPole.mapPole[13].GetLeft() < 315)
					this->mapPole.mapPole[13].SetTopLeft(this->mapPole.mapPole[13].GetLeft() + 3, this->mapPole.mapPole[13].GetTop());
			}
			else if (this->ctrlMode[index] == 2) { // closing
				if (this->mapPole.mapPole[7].GetLeft() < 1050){
					this->mapPole.mapPole[7].SetTopLeft(this->mapPole.mapPole[7].GetLeft() + 3, this->mapPole.mapPole[7].GetTop());
					// 角色在升降梯移動時，會被推走
					if (CMovingBitmap::IsOverlap(character, this->mapPole.mapPole[7]))
						character.SetTopLeft(this->mapPole.mapPole[7].GetLeft() + this->mapPole.mapPole[7].GetWidth(), character.GetTop());
				}
				if (this->mapPole.mapPole[13].GetLeft() > 175)
					this->mapPole.mapPole[13].SetTopLeft(this->mapPole.mapPole[13].GetLeft() - 3, this->mapPole.mapPole[13].GetTop());
			}
			else {
				this->mapController.controllerState[index] = false;
				this->ctrlMode[index] = 0;
			}
		}
		if (index == 4) { // red controller
			if (this->ctrlMode[index] == 2) { // closing
				if (this->mapPole.mapPole[8].GetLeft() < 1260)
					this->mapPole.mapPole[8].SetTopLeft(this->mapPole.mapPole[8].GetLeft() + 3, this->mapPole.mapPole[8].GetTop());
			}
			else if (this->ctrlMode[index] == 1) { // opening
				if (this->mapPole.mapPole[8].GetLeft() > 1085)
					this->mapPole.mapPole[8].SetTopLeft(this->mapPole.mapPole[8].GetLeft() - 3, this->mapPole.mapPole[8].GetTop());
			}
			else {
				this->mapController.controllerState[index] = false;
				this->ctrlMode[index] = 0;
			}
		}
		if (index == 5) { // blue controller
			if (this->ctrlMode[index] == 1) { // opening
				if (this->mapPole.mapPole[4].GetTop() < 140)
					this->mapPole.mapPole[4].SetTopLeft(this->mapPole.mapPole[4].GetLeft(), this->mapPole.mapPole[4].GetTop() + 3);
			}
			else if (this->ctrlMode[index] == 2) { // closing
				if (this->mapPole.mapPole[4].GetTop() > 35)
					this->mapPole.mapPole[4].SetTopLeft(this->mapPole.mapPole[4].GetLeft(), this->mapPole.mapPole[4].GetTop() - 3);
			}
			else {
				this->mapController.controllerState[index] = false;
				this->ctrlMode[index] = 0;
			}
		}
	}

	if (page == 1) {
		for (int i = 0; i < 2; i++) {
			if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapPole.mapPole[i]) && (this->fireman.character.GetTop() + this->fireman.character.GetHeight()) < (this->mapPole.mapPole[i].GetTop() + this->mapPole.mapPole[i].GetHeight() / 2))
				this->fireman.character.SetTopLeft(this->fireman.character.GetLeft(), this->mapPole.mapPole[i].GetTop() - 100);
			if (CMovingBitmap::IsOverlap(this->watergirl.character, this->mapPole.mapPole[i]) && (this->watergirl.character.GetTop() + this->watergirl.character.GetHeight()) < (this->mapPole.mapPole[i].GetTop() + this->mapPole.mapPole[i].GetHeight() / 2))
				this->watergirl.character.SetTopLeft(this->watergirl.character.GetLeft(), this->mapPole.mapPole[i].GetTop() - 100);
		}
	}
	if (page == 3) {
		for (int i = 1; i < 12; i++) {
			if (i == 5 || i == 9) continue;
			if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapPole.mapPole[i]) && (this->fireman.character.GetTop() + this->fireman.character.GetHeight()) < (this->mapPole.mapPole[i].GetTop() + this->mapPole.mapPole[i].GetHeight() / 2))
				this->fireman.character.SetTopLeft(this->fireman.character.GetLeft(), this->mapPole.mapPole[i].GetTop() - 100);
			if (CMovingBitmap::IsOverlap(this->watergirl.character, this->mapPole.mapPole[i]) && (this->watergirl.character.GetTop() + this->watergirl.character.GetHeight()) < (this->mapPole.mapPole[i].GetTop() + this->mapPole.mapPole[i].GetHeight() / 2))
				this->watergirl.character.SetTopLeft(this->watergirl.character.GetLeft(), this->mapPole.mapPole[i].GetTop() - 100);
		}
	}
	if (page == 5) {
		for (int i = 5; i < 17; i++) {
			if (i == 10 || i == 11) continue;
			if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapPole.mapPole[i]) && (this->fireman.character.GetTop() + this->fireman.character.GetHeight()) < (this->mapPole.mapPole[i].GetTop() + this->mapPole.mapPole[i].GetHeight() / 2))
				this->fireman.character.SetTopLeft(this->fireman.character.GetLeft(), this->mapPole.mapPole[i].GetTop() - 100);
			if (CMovingBitmap::IsOverlap(this->watergirl.character, this->mapPole.mapPole[i]) && (this->watergirl.character.GetTop() + this->watergirl.character.GetHeight()) < (this->mapPole.mapPole[i].GetTop() + this->mapPole.mapPole[i].GetHeight() / 2))
				this->watergirl.character.SetTopLeft(this->watergirl.character.GetLeft(), this->mapPole.mapPole[i].GetTop() - 100);
		}
	}
}

void CGameStateRun::isButtonOverlap(int page, CMovingBitmap& character) {
	if (page == 1) { // index: 0, 1
		// Button Up
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapButton.mapButton[0]) || CMovingBitmap::IsOverlap(this->watergirl.character, this->mapButton.mapButton[0])) {
			this->mapButton.mapButton[0].SetFrameIndexOfBitmap(1);
			this->mapButton.buttonState[0] = true;
		}
		else {
			this->mapButton.mapButton[0].SetFrameIndexOfBitmap(0);
			this->mapButton.buttonState[0] = false;
		}
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapButton.mapButton[1]) || CMovingBitmap::IsOverlap(this->watergirl.character, this->mapButton.mapButton[1])) {
			this->mapButton.mapButton[1].SetFrameIndexOfBitmap(1);
			this->mapButton.buttonState[1] = true;
		}
		else {
			this->mapButton.mapButton[1].SetFrameIndexOfBitmap(0);
			this->mapButton.buttonState[1] = false;
		}
		if (this->mapButton.buttonState[0] || this->mapButton.buttonState[1])
			this->movingPolefromButton(1, 1, 1, this->mapPole.mapPole[1]);
		else
			this->movingPolefromButton(1, 0, 1, this->mapPole.mapPole[1]);
	}
	
	if (page == 3) {
		// index: 0, 2, 3, 4
		// button 0
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapButton.mapButton[0]) || CMovingBitmap::IsOverlap(this->watergirl.character, this->mapButton.mapButton[0])) {
			this->mapButton.mapButton[0].SetFrameIndexOfBitmap(1);
			this->mapButton.buttonState[0] = true;
		}
		else {
			this->mapButton.mapButton[0].SetFrameIndexOfBitmap(0);
			this->mapButton.buttonState[0] = false;
		}
		// button 2
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapButton.mapButton[2]) || CMovingBitmap::IsOverlap(this->watergirl.character, this->mapButton.mapButton[2])) {
			this->mapButton.mapButton[2].SetFrameIndexOfBitmap(1);
			this->mapButton.buttonState[2] = true;
		}
		else {
			this->mapButton.mapButton[2].SetFrameIndexOfBitmap(0);
			this->mapButton.buttonState[2] = false;
		}
		// button 3
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapButton.mapButton[3]) || CMovingBitmap::IsOverlap(this->watergirl.character, this->mapButton.mapButton[3])) {
			this->mapButton.mapButton[3].SetFrameIndexOfBitmap(1);
			this->mapButton.buttonState[3] = true;
		}
		else {
			this->mapButton.mapButton[3].SetFrameIndexOfBitmap(0);
			this->mapButton.buttonState[3] = false;
		}
		// button 4
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapButton.mapButton[4]) || CMovingBitmap::IsOverlap(this->watergirl.character, this->mapButton.mapButton[4])) {
			this->mapButton.mapButton[4].SetFrameIndexOfBitmap(1);
			this->mapButton.buttonState[4] = true;
		}
		else {
			this->mapButton.mapButton[4].SetFrameIndexOfBitmap(0);
			this->mapButton.buttonState[4] = false;
		}
		// pole moving
		// button0 to pole2
		if (this->mapButton.buttonState[0])
			this->movingPolefromButton(3, 1, 2, this->mapPole.mapPole[2]);
		else
			this->movingPolefromButton(3, 0, 2, this->mapPole.mapPole[2]);

		// button2 to pole4
		if (this->mapButton.buttonState[2])
			this->movingPolefromButton(3, 1, 4, this->mapPole.mapPole[4]);
		else
			this->movingPolefromButton(3, 0, 4, this->mapPole.mapPole[4]);

		// button3 to pole1&8
		if (this->mapButton.buttonState[3] || this->mapButton.buttonState[4]) {
			this->movingPolefromButton(3, 1, 1, this->mapPole.mapPole[1]);
			this->movingPolefromButton(3, 1, 8, this->mapPole.mapPole[8]);
		}
		else {
			this->movingPolefromButton(3, 0, 1, this->mapPole.mapPole[1]);
			this->movingPolefromButton(3, 0, 8, this->mapPole.mapPole[8]);
		}
	}
}

void CGameStateRun::movingPolefromButton(int page, int mode, int index, CMovingBitmap &pole) {
	int current_Height = pole.GetTop();
	bool up = false;
	if (page == 1){
		if (mode == 1 && current_Height < 525) {
			pole.SetTopLeft(pole.GetLeft(), current_Height + 10);
		}
		if (mode == 0 && current_Height > 385) {
			pole.SetTopLeft(pole.GetLeft(), current_Height - 10);
			up = true;
		}
		if (CMovingBitmap::IsOverlap(this->fireman.character, pole) && up)
			this->fireman.character.SetTopLeft(this->fireman.character.GetLeft(), pole.GetTop() - 100);
		if (CMovingBitmap::IsOverlap(this->watergirl.character, pole) && up)
			this->watergirl.character.SetTopLeft(this->watergirl.character.GetLeft(), pole.GetTop() - 100);
	}

	if (page == 3) {
		if (mode == 1) { // opening
			if (index == 2 && pole.GetLeft() > 1015)
				pole.SetTopLeft(pole .GetLeft() - 10, pole.GetTop());
			if (index == 4 && pole.GetTop() > 350)
				pole.SetTopLeft(pole.GetLeft(), pole.GetTop() - 10);
			if (index == 1 && pole.GetLeft() < 245)
				pole.SetTopLeft(pole.GetLeft() + 10, pole.GetTop());
			if (index == 8 && pole.GetLeft() > 455)
				pole.SetTopLeft(pole.GetLeft() - 10, pole.GetTop());
		}
		if (mode == 0) { // closing
			if (index == 2 && pole.GetLeft() < 1155) 
				pole.SetTopLeft(pole.GetLeft() + 10, pole.GetTop());
			if (index == 4 && pole.GetTop() < 455) 
				pole.SetTopLeft(pole.GetLeft(), pole.GetTop() + 10);
			if (index == 1 && pole.GetLeft() > 105)
				pole.SetTopLeft(pole.GetLeft() - 10, pole.GetTop());
			if (index == 8 && pole.GetLeft() < 560)
				pole.SetTopLeft(pole.GetLeft() + 10, pole.GetTop());
		}
	}
}

void CGameStateRun::isPoolOverlap(int page, CMovingBitmap& character, CMovingBitmap& pool) {
	if (CMovingBitmap::IsOverlap(character, pool)) 
		this->page_phase = 4;
}

void CGameStateRun::isDoorOverlap(int page) {
	// Red Door
	if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapDoor.mapDoor[0])) {
		this->mapDoor.mapDoor[0].SetFrameIndexOfBitmap(1);
		this->mapDoor.doorState[0] = true;
	}
	else {
		this->mapDoor.mapDoor[0].SetFrameIndexOfBitmap(0);
		this->mapDoor.doorState[0] = false;
	}
	// Blue Door
	if (CMovingBitmap::IsOverlap(this->watergirl.character, this->mapDoor.mapDoor[1])) {
		this->mapDoor.mapDoor[1].SetFrameIndexOfBitmap(1);
		this->mapDoor.doorState[1] = true;
	}
	else {
		this->mapDoor.mapDoor[1].SetFrameIndexOfBitmap(0);
		this->mapDoor.doorState[1] = false;
	}
	// Is Game Clear
	if (this->mapDoor.doorState[0] && this->mapDoor.doorState[1]) {
		this->page_phase = 5;
	}

}

void CGameStateRun::isBoxOverlap(int page, CMovingBitmap& character) {
	// pushing Box from right side
	if (CMovingBitmap::IsOverlap(character, this->mapBox.mapBox)
		&& character.GetLeft() > this->mapBox.mapBox.GetLeft() + 25) {
		this->mapBox.push = true;
		this->mapBox.movingBox(1);
	}
	// pusing Box from left side
	else if (CMovingBitmap::IsOverlap(character, this->mapBox.mapBox)
		&& character.GetLeft() < this->mapBox.mapBox.GetLeft() - 25){
		this->mapBox.push = true;
		this->mapBox.movingBox(2);
	}
	if (this->mapBox.push)
		this->mapBox.dropDown(this->map);
}

void CGameStateRun::isDiamondOverlap(int page) {
	// red
	for (int i = 0; i < 11; i++) {
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapDiamond.redDiamond[i]))
			this->mapDiamond.redState[i] = false;
	}
	// blue
	for (int i = 0; i < 11; i++) {
		if (CMovingBitmap::IsOverlap(this->watergirl.character, this->mapDiamond.blueDiamond[i]))
			this->mapDiamond.blueState[i] = false;
	}
	if (page == 5) {
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapDiamond.whiteDiamond) 
			|| CMovingBitmap::IsOverlap(this->watergirl.character, this->mapDiamond.whiteDiamond))
			this->mapDiamond.whiteState = false;
	}
}

void CGameStateRun::isFanOverlap() {
	for (int i = 0; i < 2; i++) {
		if (CMovingBitmap::IsOverlap(this->fireman.character, this->mapFan.mapFan[i]))
			this->fireman_cl = clock_type::now();
		if (CMovingBitmap::IsOverlap(this->watergirl.character, this->mapFan.mapFan[i]))
			this->watergirl_cl = clock_type::now();
	}
	if (std::chrono::duration_cast<time_type>(clock_type::now() - this->fireman_cl).count() < 1500
		&& this->fireman.character.GetTop() > 60 && this->fireman.character.GetLeft() > 1210)
		this->fireman.character.SetTopLeft(this->fireman.character.GetLeft(), this->fireman.character.GetTop() - 13);
	
	if (std::chrono::duration_cast<time_type>(clock_type::now() - this->watergirl_cl).count() < 1500
		&& this->watergirl.character.GetTop() > 60 && this->watergirl.character.GetLeft() < 184 )
		this->watergirl.character.SetTopLeft(this->watergirl.character.GetLeft(), this->watergirl.character.GetTop() - 13);
}

void CGameStateRun::runOverlap(int page) {
	if (page == 1) {
		this->isBoxOverlap(this->page_phase - 5, this->fireman.character);
		this->isBoxOverlap(this->page_phase - 5, this->watergirl.character);
		if (this->invincible % 2 == 0) {
			this->isPoolOverlap(this->page_phase - 5, this->fireman.character, this->mapPool.greenPool);
			this->isPoolOverlap(this->page_phase - 5, this->fireman.character, this->mapPool.bluePool[0]);
			this->isPoolOverlap(this->page_phase - 5, this->watergirl.character, this->mapPool.redPool[0]);
			this->isPoolOverlap(this->page_phase - 5, this->watergirl.character, this->mapPool.greenPool);
		}
	}
	if (page == 2) {
		if (this->invincible % 2 == 0){
			for (int i = 1; i < 5; i++) {
				this->isPoolOverlap(this->page_phase - 5, this->fireman.character, this->mapPool.bluePool[i]);
				this->isPoolOverlap(this->page_phase - 5, this->watergirl.character, this->mapPool.redPool[i]);
			}
		}
	}
	if (page == 4) {
		if (this->invincible % 2 == 0){
			for (int i = 5; i < 8; i++) {
				this->isPoolOverlap(this->page_phase - 5, this->fireman.character, this->mapPool.bluePool[i]);
				this->isPoolOverlap(this->page_phase - 5, this->watergirl.character, this->mapPool.redPool[i]);
			}
		}
		this->isFanOverlap();
	}

	this->isControllerOverlap(this->page_phase - 5, this->fireman.character);
	this->isControllerOverlap(this->page_phase - 5, this->watergirl.character);
	this->isDoorOverlap(this->page_phase - 5);
	this->isDiamondOverlap(this->page_phase - 5);
	this->isButtonOverlap(this->page_phase - 5, this->fireman.character);
}