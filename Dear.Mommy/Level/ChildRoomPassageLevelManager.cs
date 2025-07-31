using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChildRoomPassageLevelManager : MonoBehaviour
{
    [SerializeField] private GameObject defaultDialogPanel;
    [SerializeField] private GameObject lookWindowFirstDialogPanel;
    [SerializeField] private GameObject lookWindowAgainDialogPanel;

    [SerializeField] private List<string> dialogListOnLookWindowFirst;
    [SerializeField] private List<string> dialogListOnLookWindowAgain;
    [SerializeField] private List<string> dialogListOnPassingChildRoom;
    [SerializeField] private List<string> dialogListOnBabySound;
    [SerializeField] private List<string> dialogListOnDoorInteraction;

    [SerializeField] private GameObject exitPortal;

    [SerializeField] private AudioSource babyLaughSound;
    [SerializeField] private AudioSource babyMamaSound;
    [SerializeField] private AudioSource bigScareSound;

    private DialogManager dialogManager;
    private bool isFirstTimeLookingWindow, hasSeenMonster;

    void Awake()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
        isFirstTimeLookingWindow = true;
        hasSeenMonster = false;
    }

    void Start()
    {
        if (!GameManager.GetInstance().isChildRoomEventSeen)
            exitPortal.SetActive(false);
    }

    public void OnLookInTheWindow()
    {
        if (!GameManager.GetInstance().isChildRoomEventSeen)
        {
            if (isFirstTimeLookingWindow)
            {
                dialogManager.ShowDialog(dialogListOnLookWindowFirst, lookWindowFirstDialogPanel, null);
                isFirstTimeLookingWindow = false;
            }
            else if (!hasSeenMonster)
            {
                bigScareSound.Play();
                babyMamaSound.Play();
                dialogManager.ShowDialog(dialogListOnLookWindowAgain, lookWindowAgainDialogPanel, null);
                exitPortal.SetActive(true);
                hasSeenMonster = true;
                GameManager.GetInstance().isChildRoomEventSeen = true;
            }
        }
    }

    public void OnPassingChildRoom()
    {
        if (!GameManager.GetInstance().isChildRoomEventSeen)
        {
            if (isFirstTimeLookingWindow)
                dialogManager.ShowDialog(dialogListOnPassingChildRoom, defaultDialogPanel, null);
            else if (!hasSeenMonster)
            {
                babyLaughSound.Play();
                dialogManager.ShowDialog(dialogListOnBabySound, defaultDialogPanel, null);
            }
        }
    }

    public void OnInteractDoor()
    {
        dialogManager.ShowDialog(dialogListOnDoorInteraction, defaultDialogPanel, null);
    }
}
