using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LockedDoorBehavior : DoorBehavior, IPuzzle
{
    [SerializeField] private float SuccessToLoadSceneDelay = 0.5f;
    [SerializeField] private string password;
    [SerializeField] private TMPro.TMP_InputField passwordInputField;

    protected override void Awake()
    {
        base.Awake();
        passwordInputField.onSubmit.AddListener(delegate { CheckAnswer(); });
        passwordInputField.onDeselect.AddListener(delegate { passwordInputField.gameObject.SetActive(false); });
    }

    public override void OnInteract()
    {
        if (!passwordInputField.gameObject.activeInHierarchy)
        {
            passwordInputField.gameObject.SetActive(true);
            Debug.Log("test!");
            passwordInputField.Select();
            Debug.Log("test2!");
            passwordInputField.ActivateInputField();
            Debug.Log("test3!");
            Initialize();
        }
    }

    public void Initialize()
    {
        passwordInputField.text = "";
    }

    public void CheckAnswer()
    {
        Debug.Log("LockedDoorBehavior - CheckAnswer");
        if (password.CompareTo(passwordInputField.text) == 0) // 비밀번호가 일치하는 경우
            OnSuccess();
        else
            OnFail();
    }

    public void OnFail()
    {
        Debug.Log("LockedDoorBehavior - OnFail");
        passwordInputField.gameObject.SetActive(false);
    }

    public void OnSuccess()
    {
        Debug.Log("LockedDoorBehavior - OnSuccess");
        passwordInputField.gameObject.SetActive(false);
        spriteRenderer.sprite = openDoorSprite;
        doorOpenSound.Play();
        if (loadSceneWhenOpen)
            Invoke("LoadScene", SuccessToLoadSceneDelay);
    }

    private void LoadScene()
    {
        GameManager.GetInstance().LoadScene(sceneToLoadOnOpen, spawnPointIndex);
    }

    public void OnTimeOut()
    {
    }

    public void CalcRemainingTiime()
    {
    }
}
