#include "stdafx.h"
#include "Player.h"

bool CPlayer::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CPlayer::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CTelegram Telegram;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			Telegram.SetMsg(MESSAGE_TYPE::Msg_Key_Down_Evasion);
			break;
		case VK_SHIFT:
			Telegram.SetMsg(MESSAGE_TYPE::Msg_Key_Down_Run);
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			Telegram.SetMsg(MESSAGE_TYPE::Msg_Key_Up_Evasion);
			break;
		case VK_SHIFT:
			Telegram.SetMsg(MESSAGE_TYPE::Msg_Key_Up_Run);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	m_pStateMachine->HandleMessage(this, Telegram);

	return false;
}
