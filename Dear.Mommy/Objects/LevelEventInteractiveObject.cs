using UnityEngine;
using UnityEngine.Events;

public class LevelEventInteractiveObject : MonoBehaviour, IInteractiveObject
{
    [SerializeField] private UnityEvent onInteractEvent;
    public void OnInteract()
    {
        onInteractEvent?.Invoke();
    }
}
