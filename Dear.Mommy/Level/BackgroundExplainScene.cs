using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class BackgroundExplainScene : MonoBehaviour
{
    [SerializeField] private InputActionAsset inputActionAsset;
    private InputActionMap menuActionMap;
    private InputAction submitAction;

    [SerializeField] private List<string> scriptList;
    [SerializeField] private int scriptIndex;
    [SerializeField] private TMPro.TMP_Text scriptText;
    [SerializeField] private string sceneToLoadOnScriptEnd;

    void Awake()
    {
        menuActionMap = inputActionAsset.FindActionMap("menu", true);
        submitAction = menuActionMap.FindAction("Submit", true);

        scriptIndex = 0;
        scriptText.text = scriptList[scriptIndex];
    }

    void OnEnable()
    {
        menuActionMap.Enable();
        submitAction.performed += ShowNextScript;
    }

    void OnDisable()
    {
        menuActionMap.Disable();
        submitAction.performed -= ShowNextScript;
    }
    
    void ShowNextScript(InputAction.CallbackContext context)
    {
        scriptIndex++;
        if (scriptIndex < scriptList.Count)
            scriptText.text = scriptList[scriptIndex];
        else
            if (!string.IsNullOrEmpty(sceneToLoadOnScriptEnd))
        {
            Debug.Log("Move to next Scene");
            GameManager.GetInstance().LoadScene(sceneToLoadOnScriptEnd);
        }
    }
}
