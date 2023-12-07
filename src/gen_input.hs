import           System.Environment (getArgs)

-- Binary tree
data T = L | N T Int T deriving Show

-- generate a list of inorder-labelled trees of size n
gen_inorder :: Int -> [T]
gen_inorder n = g 1 n
  where
  g :: Int {- from -} -> Int {- to -} -> [T]
  g m n
   | m > n     = [L]
   | otherwise = [N l k r|
                   k <- [m..n],
                   l <- g m (k-1),
                   r <- g (k+1) n]

-- generate a list of preorder-labelled trees of size n
gen_preorder :: Int -> [T]
gen_preorder n = g 1 n
  where
  g :: Int {- from -} -> Int {- to -} -> [T]
  g m n
   | m > n     = [L]
   | otherwise = [N l m r|
                   k <- [m..n],
                   l <- g (m+1) k,
                   r <- g (k+1) n]

-- Tree to preorder traversal
preorder :: T -> [Int]
preorder t = p t []
  where
  p :: T -> [Int] -> [Int]
  p L ts = ts
  p (N l n r) ts = n : p l (p r ts)

-- Tree to inorder traversal
inorder :: T -> [Int]
inorder t = i t []
  where
  i :: T -> [Int] -> [Int]
  i L ts = ts
  i (N l n r) ts = i l (n : i r ts)

test_preorder n = mapM_ (putStrLn . show) $ map preorder $ gen_inorder  n
test_inorder  n = mapM_ (putStrLn . show) $ map inorder  $ gen_preorder n

main :: IO ()
main = do
  args <- getArgs
  let n = read (args !! 0)
  let ts = gen_inorder n
  let ps = map preorder ts
  mapM_ (putStrLn . init . tail . show) ps

-- map preorder (gen 3)
