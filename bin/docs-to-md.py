   import pymupdf4llm
   import pathlib

   pathlib.Path("maxapi.md").write_text(
       pymupdf4llm.to_markdown("maxapi.pdf"),
       encoding="utf-8",
   )
