using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using TMPro;

public class DialogManager : MonoBehaviour
{
    [SerializeField] private InputActionAsset inputActionAsset;
    private InputActionMap menuActionMap;
    private InputActionMap fieldActionMap;
    private InputAction submitAction;

    GameObject dialogPanel;
    private TextMeshProUGUI dialogText;
    private List<string> dialogList;
    private int dialogIndex;
    private bool isShowingDialog;
    private Action onDialogEnd;

    void Awake()
    {
        menuActionMap = inputActionAsset.FindActionMap("menu", true);
        fieldActionMap = inputActionAsset.FindActionMap("field", true);
        submitAction = menuActionMap.FindAction("Submit", true);

        dialogPanel = null;
        dialogList = null;
        onDialogEnd = null;

        isShowingDialog = false;
        dialogIndex = 0;
    }

    void OnEnable()
    {
        submitAction.performed += OnSubmit;
    }

    void OnDisable()
    {
        menuActionMap.Disable();
        submitAction.performed -= OnSubmit;
    }

    public void ShowDialog(List<string> newDialogList, GameObject newDialogPanel, System.Action newOnDialogEnd)
    {
        Debug.Log("DialogManager - ShowDialog");
        dialogIndex = -1;
        isShowingDialog = true;

        dialogList = newDialogList;
        dialogPanel = newDialogPanel;    
        dialogPanel.SetActive(true);
        dialogText = newDialogPanel.GetComponentInChildren<TextMeshProUGUI>();
        onDialogEnd = newOnDialogEnd;

        fieldActionMap.Disable();
        menuActionMap.Enable();
        ShowNextDialogPage();

        Debug.Log($"DialogManager - ShowDialog: menuActionMap = {menuActionMap.enabled}");
    }

    public void ShowDialogNoText(GameObject newDialogPanel, System.Action newOnDialogEnd)
    {
        Debug.Log("DialogManager - ShowDialog");
        dialogIndex = -1;
        isShowingDialog = true;

        dialogList = null;
        dialogPanel = newDialogPanel;
        dialogPanel.SetActive(true);
        dialogText = null;
        onDialogEnd = newOnDialogEnd;

        fieldActionMap.Disable();
        menuActionMap.Enable();
    }

    public void ShowDialogNoTextUnableAction(GameObject newDialogPanel, System.Action newOnDialogEnd)
    {
        Debug.Log("DialogManager - ShowDialog");
        dialogIndex = -1;
        isShowingDialog = true;

        dialogList = null;
        dialogPanel = newDialogPanel;
        dialogPanel.SetActive(true);
        dialogText = null;
        onDialogEnd = newOnDialogEnd;

        fieldActionMap.Disable();
        menuActionMap.Disable();
        newOnDialogEnd?.Invoke();
    }

    private void OnSubmit(InputAction.CallbackContext context)
    {
        Debug.Log("DialogManager - OnSubmit");
        if (isShowingDialog)
            ShowNextDialogPage();
    }

    private void ShowNextDialogPage()
    {
        Debug.Log("DialogManager - ShowNextDialogPage");
        dialogIndex++;

        if (dialogList != null && dialogIndex < dialogList.Count)
            dialogText.text = dialogList[dialogIndex];
        else
        {
            Debug.Log("DialogManager - ShowNextDialogPage: EndDialog");
            fieldActionMap.Enable();
            menuActionMap.Disable();

            dialogPanel.SetActive(false);
            isShowingDialog = false;
            onDialogEnd?.Invoke();
        }
    }
}
