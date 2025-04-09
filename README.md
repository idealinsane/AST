# AST Analyzer

이 프로젝트는 C 언어 코드의 추상 구문 트리(Abstract Syntax Tree, AST)를 분석하는 도구입니다.

## 🔧 빌드 방법

`analyzer.c` 파일을 컴파일하여 실행 파일을 생성합니다:

```bash
gcc -o analyzer analyzer.c
```

🚀 실행 방법

분석 대상 AST 파일(ast.json)을 인자로 전달하여 실행합니다:

```bash
./analyzer ast.json
```

📂 입력 파일 형식

ast.json은 C 코드의 추상 구문 트리를 JSON 형식으로 저장한 파일입니다.

🧠 주요 기능

• 함수 정의(Function Definition) 추출  
• 변수 선언(Variable Declaration) 분석  
• 조건문 및 반복문 구조 탐색  
• AST 순회 및 노드 타입별 처리

✅ 요구 사항

• C 컴파일러 (예: gcc)  
• JSON 형식의 AST 파일
