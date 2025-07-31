using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Playables;

public class EscapeLevelManager : MonoBehaviour
{
    [SerializeField] private PlayableDirector playableDirector;
    [SerializeField] private GameObject dialogPanel;
    [SerializeField] private List<string> dialogList;

    private DialogManager dialogManager;
    private GameManager gameManager;

    void Start()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
        gameManager = GameManager.GetInstance();
    }
    public void PlayTimeline()
    {
        if (!gameManager.isEscapeSceneEventSeen)
        {
            gameManager.isEscapeSceneEventSeen = true;
            playableDirector.Play();
        }
    }

    public void ShowDialog()
    {
        dialogManager.ShowDialog(dialogList, dialogPanel, StopMonsterChasing);
    }

    private void StopMonsterChasing()
    {
        GameManager.GetInstance().isMonsterChasingPlayer = false;
    }
}
