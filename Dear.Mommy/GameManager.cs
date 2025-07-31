using UnityEngine;
using UnityEngine.SceneManagement;

public class GameManager
{
    private static GameManager instance;

    private int currentDiaryAmount;
    public const int maxDiaryAmount = 6; // 최대 일기 개수

    private bool isGamePaused; // 게임 일시정지 상태
    public bool IsGamePaused { get { return isGamePaused; } } // 일시정지 상태 Getter

    private MenuInputProcessor menuInputProcessor;
    private PauseMenu pauseMenu;

    private int nextSpawnPoint;
    public int NextSpawnPoint { get { return nextSpawnPoint; } }
    public bool hasFuneralKey, hasPrincipleRoomKey, isTileQuizSolved, isPrincipleRoomEventSeen, isChildRoomEventSeen, isFirstRoomEventSeen, isLobbyLevelVIsited, isChildRoomPassageOpen, isEscapeSceneEventSeen;
    public bool isMonsterChasingPlayer;

    public static GameManager GetInstance() // 나중엔 게임 시작 시 타이틀 씬에서 불러오도록 수정 
    {
        if (instance == null)
        {
            Debug.Log("GameManager - GetInstance: Creates New GameManager.");
            instance = new GameManager();
            SceneManager.sceneLoaded += instance.Init;
            instance.Init(SceneManager.GetActiveScene(), LoadSceneMode.Single);
        }
        return instance;
    }

    public void Init(Scene scene, LoadSceneMode mode)
    {
        Debug.Log("GameManager - Init");
        menuInputProcessor = GameObject.FindAnyObjectByType<MenuInputProcessor>(FindObjectsInactive.Include);
        pauseMenu = GameObject.FindAnyObjectByType<PauseMenu>(FindObjectsInactive.Include);
        if (pauseMenu != null)
            pauseMenu.gameObject.SetActive(false);

        Pathfinding.GetInstance().Init();
    }

    public void StartGame(bool startNewGame)
    {
        if (startNewGame)
            LoadNewGame();
    }

    public void PauseGame()
    {
        Debug.Log("GameManager - PauseGame");
        if (!isGamePaused)
        {
            isGamePaused = true;
            Time.timeScale = 0f;

            if (pauseMenu != null)
                pauseMenu.gameObject.SetActive(true);
            if (menuInputProcessor != null && menuInputProcessor.enabled)
                menuInputProcessor.OnPause();
        }
    }

    public void UnpauseGame()
    {
        Debug.Log("GameManager - UnpauseGame");
        if (isGamePaused)
        {
            isGamePaused = false;
            Time.timeScale = 1f;

            if (pauseMenu != null)
                pauseMenu.gameObject.SetActive(false);
            if (menuInputProcessor != null && menuInputProcessor.enabled)
                menuInputProcessor.OnUnpause();
        }
    }

    private void LoadNewGame()
    {
        hasFuneralKey = false;
        hasPrincipleRoomKey = false;
        isTileQuizSolved = false;
        isPrincipleRoomEventSeen = false;
        isChildRoomEventSeen = false;
        isFirstRoomEventSeen = false;
        isLobbyLevelVIsited = false;
        isChildRoomPassageOpen = false;
        isEscapeSceneEventSeen = true;

        isMonsterChasingPlayer = false;
        PCBehavior.isAlreadyVisited = false;

        nextSpawnPoint = 0;
    }

    public void LoadScene(string newSceneName)
    {
        Debug.Log(newSceneName);
        SceneManager.LoadScene(newSceneName, LoadSceneMode.Single);
        nextSpawnPoint = 0;
    }

    public void LoadScene(string newSceneName, int spawnPoint)
    {
        Debug.Log(newSceneName);
        SceneManager.LoadScene(newSceneName, LoadSceneMode.Single);
        nextSpawnPoint = spawnPoint;
    }
}
