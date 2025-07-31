using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PortalBehavior : MonoBehaviour
{
    [SerializeField] private string sceneToLoad;
    [SerializeField] private int spawnPointIndex = 0;

    void OnCollisionEnter2D(Collision2D collision)
    {
        Debug.Log("PortalBehavior - CollisionEnter2D");
        if (!string.IsNullOrEmpty(sceneToLoad) && collision.gameObject.CompareTag("Player")) // 닿은 물체가 플레이어인 경우
            GameManager.GetInstance().LoadScene(sceneToLoad, spawnPointIndex);
    }
}
