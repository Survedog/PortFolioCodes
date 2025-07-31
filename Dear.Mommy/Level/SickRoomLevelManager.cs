using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SickRoomLevelManager : MonoBehaviour
{
    [SerializeField] private string sceneToLoadOnDialogEnd;

    [SerializeField] private GameObject startDialogPanel;
    [SerializeField] private GameObject doorNamePlateDialogPanel;

    [SerializeField] private List<string> dialogListOnStart;
    [SerializeField] private List<string> dialogListOnDoorInteract;

    [SerializeField] private AudioSource doorOpenSound;

    private DialogManager dialogManager;

    void Awake()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
    }

    void Start()
    {
        if (!GameManager.GetInstance().isFirstRoomEventSeen)
            dialogManager.ShowDialog(dialogListOnStart, startDialogPanel, null);
    }

    public void ShowDoorNamePlateDialog()
    {
        if (!GameManager.GetInstance().isFirstRoomEventSeen)
        {
            GameManager.GetInstance().isFirstRoomEventSeen = true;
            dialogManager.ShowDialog(dialogListOnDoorInteract, doorNamePlateDialogPanel,
                () =>
                {
                    doorOpenSound.Play();
                    if (!string.IsNullOrEmpty(sceneToLoadOnDialogEnd))
                        GameManager.GetInstance().LoadScene(sceneToLoadOnDialogEnd, 0);
                });
        }
        else
        {
            doorOpenSound.Play();
            if (!string.IsNullOrEmpty(sceneToLoadOnDialogEnd))
                GameManager.GetInstance().LoadScene(sceneToLoadOnDialogEnd, 0);
        }
    }
}
