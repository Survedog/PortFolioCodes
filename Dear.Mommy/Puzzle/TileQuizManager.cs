using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class TileQuizManager : MonoBehaviour, IPuzzle
{
    [SerializeField] private GameObject[] eyeArray;
    [SerializeField] private Sprite closedEyeSprite;
    [SerializeField] private Sprite openEyeSprite;

    [SerializeField] private TileBase activeClosedEyeTile;
    [SerializeField] private TileBase inactiveClosedEyeTile;
    [SerializeField] private TileBase inactiveOpenEyeTile;
    [SerializeField] private TileBase activeOpenEyeTile;
    [SerializeField] private TileBase invalidTileBarrierTile;

    [SerializeField] private GameObject cakeBarrier;
    [SerializeField] private string sceneToLoadOnSuccess;

    [SerializeField] private List<string> dialogListOnStart;
    [SerializeField] private List<string> dialogListOnSuccess;
    [SerializeField] private DialogManager dialogManager;
    [SerializeField] private GameObject dialogPanel;

    [SerializeField] private AudioSource tilePressSound;
    [SerializeField] private AudioSource successSound;

    List<Vector3Int> activeTileCoordsList;
    private List<Vector3Int> previousCandidateTileList;

    private Tilemap openEyeTilemap, closedEyeTilemap, invalidTileBarrierTilemap;

    private SpriteRenderer[] eyeSpriteRenderArray;
    private int openEyeCount;

    private static Vector3Int[] candidateTileOffsets = { new Vector3Int(-1, 0), new Vector3Int(1, 0), new Vector3Int(0, -1), new Vector3Int(0, 1) };

    void Awake()
    {
        activeTileCoordsList = new List<Vector3Int>();
        previousCandidateTileList = new List<Vector3Int>();
        eyeSpriteRenderArray = new SpriteRenderer[eyeArray.Length];
        for (int i = 0; i < eyeArray.Length; ++i)
            eyeSpriteRenderArray[i] = eyeArray[i].GetComponent<SpriteRenderer>();

        openEyeTilemap = GameObject.Find("OpenEyeTilemap").GetComponent<Tilemap>();
        closedEyeTilemap = GameObject.Find("ClosedEyeTilemap").GetComponent<Tilemap>();
        invalidTileBarrierTilemap = GameObject.Find("InvalidTileBarrierTilemap").GetComponent<Tilemap>();

        openEyeTilemap.CompressBounds();
        closedEyeTilemap.CompressBounds();
        invalidTileBarrierTilemap.CompressBounds();

        Initialize();
    }

    void Start()
    {
        dialogManager.ShowDialog(dialogListOnStart, dialogPanel, null);
    }

    public void OnOpenEyeTilePressed(Vector3 position)
    {
        Vector3Int tileCoords = openEyeTilemap.WorldToCell(position);
        Debug.Log($"tileCoords: {tileCoords}, HasTile: {openEyeTilemap.HasTile(tileCoords)}, Contains: {activeTileCoordsList.Contains(tileCoords)}");

        if (openEyeTilemap.HasTile(tileCoords) && !activeTileCoordsList.Contains(tileCoords))
        {
            tilePressSound.Play();
            if (openEyeCount < eyeArray.Length)
            {
                openEyeCount++;
                UpdateEyeSprites();
            }

            if (openEyeCount == eyeArray.Length)
            {
                cakeBarrier.SetActive(false);
            }

            openEyeTilemap.SetTile(tileCoords, activeOpenEyeTile);
            activeTileCoordsList.Add(tileCoords);
            UpdateInvalidTileBarrier(tileCoords);
        }
    }

    public void OnClosedEyeTilePressed(Vector3 position)
    {
        Vector3Int tileCoords = closedEyeTilemap.WorldToCell(position);
        Debug.Log($"tileCoords: {tileCoords}, HasTile: {closedEyeTilemap.HasTile(tileCoords)}, Contains: {activeTileCoordsList.Contains(tileCoords)}");

        if (closedEyeTilemap.HasTile(tileCoords) && !activeTileCoordsList.Contains(tileCoords))
        {
            tilePressSound.Play();
            if (openEyeCount > 0)
            {
                openEyeCount--;
                UpdateEyeSprites();
            }

            if (!cakeBarrier.activeInHierarchy)
            {
                cakeBarrier.SetActive(true);
            }

            closedEyeTilemap.SetTile(tileCoords, activeClosedEyeTile);
            activeTileCoordsList.Add(tileCoords);
            UpdateInvalidTileBarrier(tileCoords);
        }
    }

    private void UpdateInvalidTileBarrier(Vector3Int chosenTile)
    {
        foreach (Vector3Int previousCandidate in previousCandidateTileList)
            if (previousCandidate != chosenTile)
                invalidTileBarrierTilemap.SetTile(previousCandidate, invalidTileBarrierTile);

        previousCandidateTileList.Clear();

        foreach (Vector3Int candidateOffset in candidateTileOffsets)
        {
            Vector3Int candidate = chosenTile + candidateOffset;
            if ((openEyeTilemap.HasTile(candidate) || closedEyeTilemap.HasTile(candidate)) && !activeTileCoordsList.Contains(candidate))
            {
                invalidTileBarrierTilemap.SetTile(candidate, null);
                previousCandidateTileList.Add(candidate);
            }
        }
    }

    public void Initialize()
    {
        openEyeCount = 0;
        UpdateEyeSprites();

        activeTileCoordsList.Clear();
        previousCandidateTileList.Clear();

        openEyeTilemap.SwapTile(activeOpenEyeTile, inactiveOpenEyeTile);
        closedEyeTilemap.SwapTile(activeClosedEyeTile, inactiveClosedEyeTile);        

        foreach (Vector3Int position in invalidTileBarrierTilemap.cellBounds.allPositionsWithin)
            invalidTileBarrierTilemap.SetTile(position, invalidTileBarrierTile);

        cakeBarrier.SetActive(true);
    }


    public void OnSuccess()
    {
        if (successSound)
            successSound.Play();
        dialogManager.ShowDialog(dialogListOnSuccess, dialogPanel, null);
        GameManager.GetInstance().isTileQuizSolved = true;
        Invoke("LoadScene", 2.0f);
    }

    private void LoadScene()
    {
        GameManager.GetInstance().LoadScene(sceneToLoadOnSuccess);
    }


    void UpdateEyeSprites()
    {
        for (int i = 0; i < openEyeCount; ++i)
            eyeSpriteRenderArray[i].sprite = openEyeSprite;

        for (int i = openEyeCount; i < eyeArray.Length; ++i)
            eyeSpriteRenderArray[i].sprite = closedEyeSprite;
    }

    public void CalcRemainingTiime()
    {
    }

    public void OnTimeOut()
    {
    }

    public void OnFail()
    {
    }


    public void CheckAnswer()
    {
    }
}
