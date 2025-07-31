using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Playables;

public class FuneralLevelManager : MonoBehaviour
{
    [SerializeField] private GameObject picture;
    [SerializeField] private List<string> pictureDialogList;
    [SerializeField] private GameObject dialogPanel;
    [SerializeField] private GameObject pictureDialogPanel;

    [SerializeField] private PlayableDirector playableDirector;

    private DialogManager dialogManager;

    void Awake()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
        if (GameManager.GetInstance().isEscapeSceneEventSeen)
            picture.SetActive(false);
    }

    public void ShowPictureDialog()
    {
        dialogManager.ShowDialog(pictureDialogList, pictureDialogPanel,
            () => { Debug.Log("A");  picture.SetActive(false); playableDirector.Play(); });
    }
}
