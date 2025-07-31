using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterSpawner : MonoBehaviour
{
    [SerializeField] private float monsterSpawnDelay = 1.5f;
    private GameObject player, monster;
    private GameObject[] spawnPoints;

    void Awake()
    {
        player = GameObject.FindGameObjectWithTag("Player");
        monster = GameObject.FindObjectOfType<MonsterBehavior>(true)?.gameObject;

        GameObject[] spawnPointsUnordered = GameObject.FindGameObjectsWithTag("SpawnPoint");
        spawnPoints = new GameObject[spawnPointsUnordered.Length];

        if (spawnPoints.Length > 10)
        {
            Debug.LogError("SpawnPoints amount must be under 10");
            return;
        }

        foreach (GameObject spawnPoint in spawnPointsUnordered)
        {
            int index = spawnPoint.name[spawnPoint.name.Length - 1] - '0';
            spawnPoints[index] = spawnPoint;
        }
    }

    void Start()
    {
        player.transform.position = spawnPoints[GameManager.GetInstance().NextSpawnPoint].transform.position;
        if (monster != null && GameManager.GetInstance().isMonsterChasingPlayer)
            Invoke("SpawnMonster", monsterSpawnDelay);
    }

    void SpawnMonster()
    {
        monster.transform.position = spawnPoints[GameManager.GetInstance().NextSpawnPoint].transform.position;
        monster.SetActive(true);
    }
}
