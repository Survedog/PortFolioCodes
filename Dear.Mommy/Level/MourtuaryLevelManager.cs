using System.Collections.Generic;
using UnityEngine;

public class MourtuaryLevelManager : MonoBehaviour
{
    [SerializeField] private GameObject defaultDialogPanel;
    [SerializeField] private GameObject insideMortuaryDialogPanel;
    [SerializeField] private GameObject monsterNearWindowDialogPanel;
    [SerializeField] private GameObject monsterGoneDialogPanel;

    [SerializeField] private List<string> dialogListOnInteractMortuary;
    [SerializeField] private List<string> dialogListInsideMortuary;
    [SerializeField] private List<string> dialogListAfterExitMortuary;

    [SerializeField] private GameObject exitPortal;

    [SerializeField] private AudioSource doorOpenSound;
    [SerializeField] private AudioSource doorCloseSound;
    [SerializeField] private AudioSource insideMortuaryBackgroundSound;

    private DialogManager dialogManager;
    private bool isEventEnded;

    void Awake()
    {
        dialogManager = FindAnyObjectByType<DialogManager>();
        isEventEnded = false;
    }

    void Start()
    {
        exitPortal.SetActive(false);
    }

    public void GetInMortuary()
    {
        if (!isEventEnded)
        {
            isEventEnded = true;
            dialogManager.ShowDialog(dialogListOnInteractMortuary, defaultDialogPanel, OnInsideMortuary);

            GameManager.GetInstance().hasFuneralKey = true;
            GameManager.GetInstance().isMonsterChasingPlayer = false;
            exitPortal.SetActive(true);
        }
    }

    private void OnInsideMortuary()
    {
        insideMortuaryBackgroundSound.Play();
        dialogManager.ShowDialog(dialogListInsideMortuary, insideMortuaryDialogPanel, OnMonsterNearWindow);
    }

    private void OnMonsterNearWindow()
    {
        doorOpenSound.Play();
        dialogManager.ShowDialogNoText(monsterNearWindowDialogPanel, OnMonsterGone);
    }

    private void OnMonsterGone()
    {
        doorCloseSound.Play();
        dialogManager.ShowDialogNoText(monsterGoneDialogPanel, OnExitMortuary);
    }

    private void OnExitMortuary()
    {
        insideMortuaryBackgroundSound.Stop();
        dialogManager.ShowDialog(dialogListAfterExitMortuary, defaultDialogPanel, null);
    }
}
