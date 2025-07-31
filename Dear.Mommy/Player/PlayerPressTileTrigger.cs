using UnityEngine;

public class PlayerPressTileTrigger : MonoBehaviour
{
    TileQuizManager tileQuizManager;

    void Start()
    {
        tileQuizManager = GameObject.FindAnyObjectByType<TileQuizManager>();
    }

    void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.name == "ClosedEyeTilemap")
        {
            Debug.Log("PlayerPressTileTrigger - pressed ClosedEyeTilemap");
            tileQuizManager.OnClosedEyeTilePressed(transform.position);
        }
        else if (collision.gameObject.name == "OpenEyeTilemap")
        {
            Debug.Log("PlayerPressTileTrigger - pressed OpenEyeTilemap");
            tileQuizManager.OnOpenEyeTilePressed(transform.position);
        }
    }
}
