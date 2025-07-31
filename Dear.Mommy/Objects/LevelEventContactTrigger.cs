using UnityEngine;
using UnityEngine.Events;

public class LevelEventContactTrigger : MonoBehaviour
{
    [SerializeField] private UnityEvent onPlayerContactEvent;
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
            onPlayerContactEvent?.Invoke();
    }
}
