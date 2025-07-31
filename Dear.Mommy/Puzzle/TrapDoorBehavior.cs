using UnityEngine;

public class TrapDoorBehavior : MonoBehaviour, IInteractiveObject
{
    [SerializeField] private GameObject jumpScarePanel;
    [SerializeField] private string sceneToLoadWhenTriggered = "TrapFloorScene";
    [SerializeField] private float loadSceneDelay = 0.5f;

    public void OnInteract()
    {
        Debug.Log("TrapDoorBehavior - OpenedTrapDoor");
        if (jumpScarePanel != null)
        {
            jumpScarePanel.SetActive(true);
            Invoke("LoadScene", loadSceneDelay);
        }
    }

    void LoadScene()
    {
        GameManager.GetInstance().LoadScene(sceneToLoadWhenTriggered);
    }
}
