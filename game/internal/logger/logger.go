package logger

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sync"
	"time"
)

const (
	LevelInfo = iota
	LevelWarn
	LevelError
	LevelDebug
)

type Logger struct {
	mu          sync.Mutex
	debug       bool
	quiet       bool
	level       int
	generalOut  io.Writer
	errorOut    io.Writer
	generalFile *os.File
	errorFile   *os.File
}

var global = &Logger{
	level:      LevelInfo,
	generalOut: os.Stdout,
	errorOut:   os.Stderr,
}

func Init(debug bool, quiet bool, level int, logFile, errorFile, baseDir string) error {
	global.mu.Lock()
	defer global.mu.Unlock()

	global.closeFilesLocked()
	global.debug = debug
	global.quiet = quiet
	global.level = level
	if quiet {
		global.generalOut = io.Discard
		global.errorOut = io.Discard
	} else {
		global.generalOut = os.Stdout
		global.errorOut = os.Stderr
	}

	if logFile != "" {
		resolved := resolvePath(baseDir, logFile)
		if err := os.MkdirAll(filepath.Dir(resolved), 0o755); err != nil {
			return fmt.Errorf("failed to create log directory for %s: %w", resolved, err)
		}
		f, err := os.OpenFile(resolved, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0o644)
		if err != nil {
			return fmt.Errorf("failed to open log file %s: %w", resolved, err)
		}
		global.generalFile = f
		global.generalOut = f
	}

	if errorFile != "" {
		resolved := resolvePath(baseDir, errorFile)
		if err := os.MkdirAll(filepath.Dir(resolved), 0o755); err != nil {
			return fmt.Errorf("failed to create error log directory for %s: %w", resolved, err)
		}
		f, err := os.OpenFile(resolved, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0o644)
		if err != nil {
			return fmt.Errorf("failed to open error log file %s: %w", resolved, err)
		}
		global.errorFile = f
		global.errorOut = f
	}

	return nil
}

func Close() {
	global.mu.Lock()
	defer global.mu.Unlock()
	global.closeFilesLocked()
}

func (l *Logger) closeFilesLocked() {
	if l.generalFile != nil {
		_ = l.generalFile.Close()
		l.generalFile = nil
	}
	if l.errorFile != nil {
		_ = l.errorFile.Close()
		l.errorFile = nil
	}
}

func resolvePath(baseDir, path string) string {
	if filepath.IsAbs(path) {
		return path
	}
	if baseDir == "" {
		return path
	}
	return filepath.Clean(filepath.Join(baseDir, path))
}

func enabled(level int, configured int) bool {
	if configured >= LevelDebug {
		return true
	}

	if level == LevelDebug {
		return false
	}

	switch configured {
	case LevelError:
		return level == LevelError
	case LevelWarn:
		return level == LevelWarn || level == LevelError
	default: // LevelInfo and unknown values
		return level == LevelInfo || level == LevelWarn || level == LevelError
	}
}

func levelName(level int) string {
	switch level {
	case LevelDebug:
		return "DEBUG"
	case LevelInfo:
		return "INFO"
	case LevelWarn:
		return "WARN"
	case LevelError:
		return "ERROR"
	default:
		return "LOG"
	}
}

func write(level int, format string, args ...interface{}) {
	global.mu.Lock()
	defer global.mu.Unlock()

	shouldWrite := enabled(level, global.level)
	if !shouldWrite && !(level == LevelInfo && !global.quiet) {
		return
	}
	if level == LevelDebug && !global.debug {
		return
	}

	line := fmt.Sprintf("%s [%s] %s\n", time.Now().Format(time.RFC3339), levelName(level), fmt.Sprintf(format, args...))

	verboseConsoleOnly := global.level >= LevelDebug && !global.quiet

	if verboseConsoleOnly {
		if level == LevelError {
			_, _ = os.Stderr.Write([]byte(line))
		} else {
			_, _ = os.Stdout.Write([]byte(line))
		}
		return
	}

	if global.quiet {
		if !shouldWrite {
			return
		}
		if level == LevelError {
			if global.errorFile != nil {
				_, _ = global.errorFile.Write([]byte(line))
			}
			return
		}
		if global.generalFile != nil {
			_, _ = global.generalFile.Write([]byte(line))
		}
		return
	}

	// Info should always be visible in console unless quiet=true, even when
	// debugLevel filters it out from file output.
	if level == LevelInfo && !shouldWrite {
		_, _ = os.Stdout.Write([]byte(line))
		return
	}

	if level == LevelError {
		if global.errorFile != nil {
			_, _ = global.errorFile.Write([]byte(line))
		}
		_, _ = os.Stderr.Write([]byte(line))
		return
	}

	if global.generalFile != nil {
		_, _ = global.generalFile.Write([]byte(line))
	}
	_, _ = os.Stdout.Write([]byte(line))
}

func Debug(format string, args ...interface{}) {
	write(LevelDebug, format, args...)
}

func Info(format string, args ...interface{}) {
	write(LevelInfo, format, args...)
}

func Warn(format string, args ...interface{}) {
	write(LevelWarn, format, args...)
}

func Error(format string, args ...interface{}) {
	write(LevelError, format, args...)
}
