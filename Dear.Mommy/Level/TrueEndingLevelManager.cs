using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TrueEndingLevelManager : MonoBehaviour
{
    [SerializeField] private GameObject trueEndingDialogPanel;

    private DialogManager dialogManager;

    void Awake()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
    }


    public void OnMonsterInteraction()
    {
        dialogManager.ShowDialogNoText(trueEndingDialogPanel, LoadTrueEndingScriptScene);
    }

    void LoadTrueEndingScriptScene()
    {
        GameManager.GetInstance().LoadScene("트루엔딩");
    }
}
