using UnityEngine;

public class ClockBehavior : MonoBehaviour, IInteractiveObject
{
    [SerializeField] private GameObject clockImagePanel;
    [SerializeField] private float showImageDuration = 2.0f;

    void Start()
    {
        if (clockImagePanel != null)
            clockImagePanel.SetActive(false);
    }

    public void OnInteract()
    {
        if (clockImagePanel != null && !clockImagePanel.activeInHierarchy)
        {
            clockImagePanel.SetActive(true);
            Invoke("HideImage", showImageDuration);
        }
    }

    private void HideImage()
    {
        clockImagePanel.SetActive(false);
    }
}
