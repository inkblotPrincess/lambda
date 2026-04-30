.PHONY: docker-image config build tests runtime-dlls clean

IMAGE := lambda-mingw-gcc-trunk
BUILD_DIR := build
TOOLCHAIN := cmake/mingw-gcc-trunk.cmake

docker-image:
	docker build -t $(IMAGE) .

config:
	docker run --rm -it -v "$(CURDIR)":/workspace -w /workspace $(IMAGE) \
		cmake -S . -B $(BUILD_DIR) \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) \
		-G Ninja

build:
	docker run --rm -it -v "$(CURDIR)":/workspace -w /workspace $(IMAGE) \
		cmake --build $(BUILD_DIR)

runtime-dlls:
	docker run --rm -v "$(CURDIR)/$(BUILD_DIR)/bin":/out $(IMAGE) \
		bash -lc "find /usr/local -name '*.dll' -exec cp -v {} /out/ \;"

run: build runtime-dlls
	powershell -NoProfile -Command "$$exe = '$(BUILD_DIR)\bin\lambda.exe'; if (!(Test-Path $$exe)) { Write-Error 'Executable not found'; exit 1 }; & $$exe $(ARGS)"

tests: build runtime-dlls
	powershell -NoProfile -Command "$$exe = '$(BUILD_DIR)\bin\lambdatests.exe'; if (!(Test-Path $$exe)) { Write-Error 'Test executable not found'; exit 1 }; & $$exe --gtest_color=yes"

clean:
	powershell -NoProfile -Command "Remove-Item -Recurse -Force '$(BUILD_DIR)' -ErrorAction SilentlyContinue"