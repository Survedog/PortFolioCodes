using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LobbyLevelManager : MonoBehaviour
{
    [SerializeField] private List<string> enterLobbyDialogList;
    [SerializeField] private List<string> funeralDoorLockedDialogList;
    [SerializeField] private List<string> funeralDoorOpenDialogList;
    [SerializeField] private List<string> childRoomPassageOpenDialogList;
    [SerializeField] private GameObject dialogPanel;

    [SerializeField] private GameObject childRoomPassageBarrier;
    [SerializeField] private AudioSource childRoomPassageOpenSound;
    [SerializeField] private AudioSource doorOpenSound;

    private DialogManager dialogManager;
    private GameManager gameManager;

    void Awake()
    {
        gameManager = GameManager.GetInstance();
        dialogManager = FindAnyObjectByType<DialogManager>();        
    }

    void Start()
    {
        if (!gameManager.isLobbyLevelVIsited)
        {
            dialogManager.ShowDialog(enterLobbyDialogList, dialogPanel, null);
            gameManager.isLobbyLevelVIsited = true;
        }

        if (gameManager.isTileQuizSolved)
        {
            if (!gameManager.isChildRoomPassageOpen)
            {
                gameManager.isChildRoomPassageOpen = true;
                childRoomPassageOpenSound.Play();
                dialogManager.ShowDialog(childRoomPassageOpenDialogList, dialogPanel, null);
            }
            childRoomPassageBarrier.SetActive(false);
        }
    }

    public void OnComputerInteraction()
    {
        if (!gameManager.isTileQuizSolved)
            gameManager.LoadScene("타일퀴즈");
        else
            gameManager.LoadScene("로비컴퓨터");
    }

    public void OnFuneralDoorInteraction()
    {
        if (!gameManager.hasFuneralKey)
            dialogManager.ShowDialog(funeralDoorLockedDialogList, dialogPanel, null);
        else
            dialogManager.ShowDialog(funeralDoorOpenDialogList, dialogPanel, LoadFuneralScene);
    }

    public void OnSickRoomDoorInteraction()
    {
        doorOpenSound.Play();
        if (!gameManager.isEscapeSceneEventSeen)
            gameManager.LoadScene("입원실", 1);
        else
            gameManager.LoadScene("입원실_트루엔딩", 1);
    }

    private void LoadFuneralScene()
    {
        gameManager.LoadScene("장례식장", 0);
    }
}
