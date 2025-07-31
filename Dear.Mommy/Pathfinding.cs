using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class Pathfinding
{
    private static Pathfinding instance;

    private int walkableTileAmount;
    private Tilemap walkableTileMap;
    private static Vector2Int[] neighborOffset = { new Vector2Int(-1, 0), new Vector2Int(1, 0), new Vector2Int(0, -1), new Vector2Int(0, 1) };

    private HashSet<Vector2Int> closedNodeSet;
    MinHeap<WalkableTileInfo> tilePriorityQueue;

    private class WalkableTileInfo : System.IComparable<WalkableTileInfo>
    {
        public Vector2Int position;
        public WalkableTileInfo previousTile;
        public bool isInStraightDirection;
        public int currentDistance;
        public int remainingDistance;

        public WalkableTileInfo(Vector2Int position, WalkableTileInfo previousTile, bool isInStraightDirection, int currentDistance, int remainingDistance)
        {
            this.position = position;
            this.previousTile = previousTile;
            this.isInStraightDirection = isInStraightDirection;
            this.currentDistance = currentDistance;
            this.remainingDistance = remainingDistance;
        }

        public int CompareTo(WalkableTileInfo other)
        {
            if (currentDistance + remainingDistance > other.currentDistance + other.remainingDistance)
                return 1;
            else if (currentDistance + remainingDistance < other.currentDistance + other.remainingDistance)
                return -1;
            else
            {
                if (isInStraightDirection != other.isInStraightDirection)
                {
                    if (isInStraightDirection)
                        return -1;
                    else
                        return 1;
                }
                return 0;
            }
        }
    }

    public class Vector2IntComparer : IEqualityComparer<Vector2Int>
    {
        public static readonly Vector2IntComparer Default = new Vector2IntComparer();

        public bool Equals(Vector2Int x, Vector2Int y)
        {
            return x == y;
        }

        public int GetHashCode(Vector2Int obj)
        {
            return obj.GetHashCode();
        }
    }

    private Pathfinding()
    {
        Init();
    }

    public static Pathfinding GetInstance()
    {
        if (instance == null)
            instance = new Pathfinding();

        return instance;
    }

    public void Init()
    {
        Debug.Log("Pathfinding - Init");
        GameObject walkableTileMapObject = GameObject.Find("WalkableTilemap"); 

        if (walkableTileMapObject)
        {
            walkableTileMap = walkableTileMapObject.GetComponent<Tilemap>();
            walkableTileMap.CompressBounds();
            walkableTileAmount = 0;

            foreach (Vector3Int position in walkableTileMap.cellBounds.allPositionsWithin)
            {
                if (walkableTileMap.HasTile(position))
                    walkableTileAmount++;
            }
            Debug.Log($"Pathfinding - WalkableTileAmount: {walkableTileAmount}");

            closedNodeSet = new HashSet<Vector2Int>(walkableTileAmount);
            tilePriorityQueue = new MinHeap<WalkableTileInfo>(walkableTileAmount);
        }
    }

    public void FindPath(Vector3 worldStart, Vector3 worldDest, Stack<Vector3> pathStack)
    {
        Vector2Int startCell = (Vector2Int)walkableTileMap.WorldToCell(worldStart);
        Vector2Int destCell = (Vector2Int)walkableTileMap.WorldToCell(worldDest);

        FindPath(startCell, destCell, pathStack);

        Stack<Vector3> debugPathStack = new Stack<Vector3>(pathStack);
    }

    private void FindPath(Vector2Int startCell, Vector2Int destCell, Stack<Vector3> pathStack)
    {
        closedNodeSet.Clear();
        tilePriorityQueue.Clear();

        bool finished = false;
        WalkableTileInfo pathInfo = null;
        tilePriorityQueue.Add(new WalkableTileInfo(startCell, null, false,  0, Math.Abs(startCell.x - destCell.x) + Math.Abs(startCell.y - destCell.y)));

        while (!finished)
        {
            if (tilePriorityQueue.Empty())
                break;

            WalkableTileInfo currentTile = tilePriorityQueue.Top();
            tilePriorityQueue.Pop();
            closedNodeSet.Add(currentTile.position);

            foreach (Vector2Int offset in neighborOffset)
            {
                Vector2Int neighbor = currentTile.position + offset;

                if (closedNodeSet.Contains(neighbor))
                    continue;
                else
                    closedNodeSet.Add(neighbor);

                if (walkableTileMap.HasTile((Vector3Int)neighbor))
                {
                    if (neighbor == destCell)
                    {
                        finished = true;
                        pathInfo = new WalkableTileInfo(neighbor, currentTile, false, currentTile.currentDistance + 1, Math.Abs(neighbor.x - destCell.x) + Math.Abs(neighbor.y - destCell.y));
                        break;
                    }

                    tilePriorityQueue.Add(new WalkableTileInfo(neighbor, currentTile,
                        (currentTile.previousTile != null && currentTile.position - currentTile.previousTile.position == offset),
                        currentTile.currentDistance + 1,
                        Math.Abs(neighbor.x - destCell.x) + Math.Abs(neighbor.y - destCell.y)));
                }
            }
        }

        pathStack.Clear();
        while (pathInfo != null)
        {
            pathStack.Push(walkableTileMap.GetCellCenterWorld((Vector3Int)pathInfo.position));
            pathInfo = pathInfo.previousTile;
        }
    }

    public static void DrawPathDebugLine(Stack<Vector3> pathStack)
    {
        Vector3 lineStart, lineEnd;

        if (pathStack.Count <= 0)
            return;

        lineEnd = pathStack.Pop();
        while (pathStack.Count > 0)
        {
            lineStart = lineEnd;
            if (pathStack.Count > 0)
                lineEnd = pathStack.Pop();
            else
                break;

            Debug.DrawLine(lineStart, lineEnd, Color.blue, 0.2f);
        }
    }
}
