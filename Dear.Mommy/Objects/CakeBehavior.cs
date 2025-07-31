using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CakeBehavior : MonoBehaviour
{
    TileQuizManager tileQuizManager;
    private bool IsObtained;

    void Start()
    {
        IsObtained = false;
        tileQuizManager = GameObject.FindAnyObjectByType<TileQuizManager>();
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (!IsObtained)
        {            
            tileQuizManager.OnSuccess();
            IsObtained = true;
            gameObject.SetActive(false);
        }
    }
}
