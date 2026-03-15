#!/bin/bash
#
# Script to generate PDF documentation from HTML files
# KY-002 Vibration Sensor Documentation Generator
#

# Set color codes for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOCS_DIR="$SCRIPT_DIR/docs"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}KY-002 PDF Documentation Generator${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if chromium is installed
if ! command -v chromium &> /dev/null; then
    echo -e "${RED}Error: chromium is not installed${NC}"
    echo "Please install it with: sudo apt install chromium-browser"
    exit 1
fi

# Check if docs directory exists
if [ ! -d "$DOCS_DIR" ]; then
    echo -e "${RED}Error: docs directory not found${NC}"
    exit 1
fi

# Change to docs directory
cd "$DOCS_DIR" || exit 1

echo -e "${BLUE}Working directory:${NC} $DOCS_DIR"
echo ""

# Generate English PDF
echo -e "${GREEN}Generating English PDF...${NC}"
if [ -f "KY002_VibrationSensor_EN.html" ]; then
    chromium --headless --disable-gpu \
             --virtual-time-budget=10000 \
             --run-all-compositor-stages-before-draw \
             --print-to-pdf=KY002_VibrationSensor_EN.pdf \
             --print-to-pdf-no-header \
             KY002_VibrationSensor_EN.html 2>&1 | grep -i "bytes written"
    if [ -f "KY002_VibrationSensor_EN.pdf" ]; then
        SIZE=$(du -h KY002_VibrationSensor_EN.pdf | cut -f1)
        echo -e "${GREEN}✓ English PDF generated successfully ($SIZE)${NC}"
    else
        echo -e "${RED}✗ Failed to generate English PDF${NC}"
    fi
else
    echo -e "${RED}✗ KY002_VibrationSensor_EN.html not found${NC}"
fi

echo ""

# Generate Romanian PDF
echo -e "${GREEN}Generating Romanian PDF...${NC}"
if [ -f "KY002_VibrationSensor_RO.html" ]; then
    chromium --headless --disable-gpu \
             --virtual-time-budget=10000 \
             --run-all-compositor-stages-before-draw \
             --print-to-pdf=KY002_VibrationSensor_RO.pdf \
             --print-to-pdf-no-header \
             KY002_VibrationSensor_RO.html 2>&1 | grep -i "bytes written"
    if [ -f "KY002_VibrationSensor_RO.pdf" ]; then
        SIZE=$(du -h KY002_VibrationSensor_RO.pdf | cut -f1)
        echo -e "${GREEN}✓ Romanian PDF generated successfully ($SIZE)${NC}"
    else
        echo -e "${RED}✗ Failed to generate Romanian PDF${NC}"
    fi
else
    echo -e "${RED}✗ KY002_VibrationSensor_RO.html not found${NC}"
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}PDF generation complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# List generated PDFs
echo -e "${BLUE}Generated files:${NC}"
ls -lh *.pdf 2>/dev/null | awk '{print $9, "-", $5}'

exit 0
