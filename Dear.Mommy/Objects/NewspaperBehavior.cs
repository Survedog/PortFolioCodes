using UnityEngine;

public class NewspaperBehavior : MonoBehaviour, IInteractiveObject
{
    [SerializeField] private GameObject newspapaerImagePanel;
    [SerializeField] private float showImageDuration = 2.0f;

    void Start()
    {
        if (newspapaerImagePanel != null)
            newspapaerImagePanel.SetActive(false);
    }

    public void OnInteract()
    {
        if (newspapaerImagePanel != null && !newspapaerImagePanel.activeInHierarchy)
        {
            newspapaerImagePanel.SetActive(true);
            Invoke("HideImage", showImageDuration);
        }
    }

    private void HideImage()
    {
        newspapaerImagePanel.SetActive(false);
    }
}
