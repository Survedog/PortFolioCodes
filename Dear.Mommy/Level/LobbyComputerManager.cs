using System.Collections.Generic;
using UnityEngine;

public class LobbyComputerManager : MonoBehaviour
{
    [SerializeField] private GameObject DocumentPanel;
    [SerializeField] private GameObject InternetPanel;    

    [SerializeField] private DialogManager dialogManager;
    [SerializeField] private GameObject dialogPanel;
    [SerializeField] private List<string> dialogListOnStart;
    [SerializeField] private List<string> dialogListOnNotEnoughPowerOff;

    private bool isDocumentChecked;
    private bool isInternetChecked;
    private string sceneToLoadOnPowerOff = "복도";

    void Start()
    {
        dialogManager.ShowDialog(dialogListOnStart, dialogPanel, null);
    }

    public void ShowDocumentPanel()
    {
        Debug.Log("LobbyComputerManager - ShowDocumentPanel");
        DocumentPanel.SetActive(true);
        isDocumentChecked = true;
    }

    public void HideDocumentPanel()
    {
        Debug.Log("LobbyComputerManager - HideDocumentPanel");
        DocumentPanel.SetActive(false);
    }

    public void ShowInternetPanel()
    {
        Debug.Log("LobbyComputerManager - ShowInternetPanel");
        InternetPanel.SetActive(true);
        isInternetChecked = true;
    }

    public void HideInternetPanel()
    {
        Debug.Log("LobbyComputerManager - HideInternetPanel");
        InternetPanel.SetActive(false);
    }

    public void OnPowerOff()
    {
        Debug.Log("LobbyComputerManager - OnPowerButtonClicked");
        if (isDocumentChecked && isInternetChecked && !string.IsNullOrEmpty(sceneToLoadOnPowerOff))
            GameManager.GetInstance().LoadScene(sceneToLoadOnPowerOff, 3);
        else
            dialogManager.ShowDialog(dialogListOnNotEnoughPowerOff, dialogPanel, null);
    }
}
