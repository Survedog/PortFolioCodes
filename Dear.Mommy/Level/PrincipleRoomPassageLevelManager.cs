using System.Collections.Generic;
using UnityEngine;

public class PrincipleRoomPassageLevelManager : MonoBehaviour
{

    [SerializeField] private GameObject defaultDialogPanel;
    [SerializeField] private List<string> dialogListOnExtinguisherInteraction;
    [SerializeField] private List<string> dialogListOnInteractDoorNokey;
    [SerializeField] private List<string> dialogListOnInteractDoorHaveKey;
    [SerializeField] private string principleRoomScene;

    private DialogManager dialogManager;

    void Awake()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
    }

    public void OnInteractExtinguisher()
    {
        if (!GameManager.GetInstance().hasPrincipleRoomKey)
        {
            dialogManager.ShowDialog(dialogListOnExtinguisherInteraction, defaultDialogPanel, null);
            GameManager.GetInstance().hasPrincipleRoomKey = true;
        }
    }

    public void OnInteractPrincipleRoomDoor()
    {
        if (GameManager.GetInstance().hasPrincipleRoomKey)
        {
            if (!GameManager.GetInstance().isPrincipleRoomEventSeen)
            {
                dialogManager.ShowDialog(dialogListOnInteractDoorHaveKey, defaultDialogPanel, LoadPrincipleRoomScene);
            }
            else
                GameManager.GetInstance().LoadScene($"{principleRoomScene}_±«¹°³ª°£ÈÄ");
        }   
        else
        {
            dialogManager.ShowDialog(dialogListOnInteractDoorNokey, defaultDialogPanel, null);
        }
    }

    private void LoadPrincipleRoomScene()
    {
        GameManager.GetInstance().LoadScene(principleRoomScene);
    }
}
